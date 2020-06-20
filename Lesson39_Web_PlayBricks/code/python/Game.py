"""""""""""""""""""""""
""""""""游戏类"""""""""
""""""""""""""""""""""""

import pygame
import setting as cfg
from Sprite import *


class Game(object):

    '''初始化游戏类'''
    def __init__(self):
        pass

    ''' 游戏初始化 '''
    def game_init(self):

        #游戏初始化
        pygame.init()
        #设置游戏屏幕大小
        self.screen = pygame.display.set_mode((cfg.sc_width, cfg.sc_height))

        #设置游戏标题
        pygame.display.set_caption("Play brick")

        #设置游戏字体
        self.font = pygame.font.Font(None, 36)

        #设置游戏时钟
        self.timer = pygame.time.Clock()

        #设置游戏相关精灵组
        self.paddle_group = pygame.sprite.Group()
        self.block_group = pygame.sprite.Group()
        self.ball_group = pygame.sprite.Group()

        #加载挡板并设置位置
        self.paddle = Sprite(self.screen)
        self.paddle.load("pic/paddle.png")
        self.paddle.set_pos((250, 460))
        self.paddle_group.add(self.paddle)

        #加载小球并设置位置
        self.ball = Sprite(self.screen)
        self.ball.load("pic/ball.png")
        self.ball.set_pos((250, 450))
        self.ball_group.add(self.ball)

    ''' 加载砖块 '''
    def load_level(self):
        #清空砖块精灵组
        self.block_group.empty();
        
        for by in range(0, 10):
            for bx in range(0, 10):
                #加载砖块并设置每个砖块的位置
                self.block = Sprite(self.screen)
                self.block.load("pic/blocks.png", 58, 29, 4)
                x = 5 + bx * (self.block.frame_width+1)
                y = 30 + by * (self.block.frame_height+1)
                self.block.set_pos((x, y))
                #检测不需要加载砖块的位置
                num = cfg.levels[cfg.level][by*10+bx]
                self.block.first_frame = num - 1
                self.block.last_frame = num - 1
                if num > 0:
                    self.block_group.add(self.block)

    ''' 进入下一关 '''
    def goto_next_level(self):
        cfg.level += 1
        if cfg.level > len(cfg.levels)-1:
            cfg.level = 0
        self.load_level()

    ''' 
    更新砖块
    param {int} ticks 自pygame.init()调用以来的毫秒数
    '''
    def update_blocks(self, ticks):
        #如果砖块数为0，进入下一关，重置小球位置和挡板位置
        if len(self.block_group) == 0:
            self.goto_next_level()
            cfg.waiting = True
        self.block_group.update(ticks, 50)

    '''
    移动挡板
    param {dict} keys 一个由布尔类型值组成的序列，表示键盘上所有按键的当前状态
    param {int} ticks 自pygame.init()调用以来的毫秒数
    '''
    def move_paddle(self, joystick, ticks):
        self.paddle_group.update(ticks, 50)

        #空格则重置小球速度，发射小球
        if joystick == "pressed":
            if cfg.waiting:
                cfg.waiting = False
                self.reset_ball()
        #挡板向左运动
        elif joystick == "left":
            #向左运动速度
            self.paddle.velocity.x = -10.0
        #挡板向右运动
        elif joystick == "right":
            #向右运动速度
            self.paddle.velocity.x = 10.0
        #挡板静止不动
        else:
            #速度为0
            self.paddle.velocity.x = 0
        #添加速度
        self.paddle.add_x(self.paddle.velocity.x)

        #设置挡板的运动范围
        #不能超过屏幕的左边，超过则重置挡板位置为最左侧
        if self.paddle.get_x() < 0:
            self.paddle.set_x(0)
        #不能超过屏幕的右边，超过则重置挡板位置为最右侧
        elif self.paddle.get_x() > cfg.sc_width-self.paddle.frame_width:
            self.paddle.set_x(cfg.sc_width-self.paddle.frame_width)

    '''
    移动小球
    param {int} ticks 自pygame.init()调用以来的毫秒数
    '''
    def move_ball(self, ticks):
        self.ball_group.update(ticks, 50)

        #如果死亡一次，小球重置会挡板中央，游戏重新开始
        if cfg.waiting:
            self.ball.set_x(self.paddle.get_x() +
                            self.paddle.frame_width/2-self.ball.frame_width/2)
            self.ball.set_y(self.paddle.get_y()-self.ball.frame_height)
        #未死亡，小球继续移动
        else:
            self.ball.add_x(self.ball.velocity.x)
            self.ball.add_y(self.ball.velocity.y)

        #超出屏幕x轴范围，触边反弹，x轴方向速度反向
        if self.ball.get_x() < 0:
            self.ball.set_x(0)
            self.ball.velocity.x *= -1
        elif self.ball.get_x() > cfg.sc_width-self.ball.frame_width:
            self.ball.set_x(cfg.sc_width-self.ball.frame_width)
            self.ball.velocity.x *= -1
        #超出屏幕上边，触边反弹，y轴方向速度反向
        if self.ball.get_y() < 0:
            self.ball.set_y(0)
            self.ball.velocity.y *= -1
        #小球掉落，生命-1，判断游戏是否结束
        elif self.ball.get_y() > cfg.sc_height-self.ball.frame_height:
            cfg.waiting = True
            cfg.lives -= 1
            if cfg.lives < 1:
                cfg.game_over = True

    ''' 重置小球速度 '''
    def reset_ball(self):
        self.ball.velocity = Point(4.5, -7.0)

    ''' 检测小球与挡板的碰撞 '''
    def collision_ball_paddle(self):
        #检测碰撞
        if pygame.sprite.collide_rect(self.ball, self.paddle):

            #y轴方向速度反向
            self.ball.velocity.y = -abs(self.ball.velocity.y)
            
            #小球及挡板中心的x及y坐标
            bx = self.ball.get_x() + self.ball.frame_width/2
            by = self.ball.get_y() + self.ball.frame_height/2
            px = self.paddle.get_x() + self.paddle.frame_width/2
            py = self.paddle.get_y() + self.paddle.frame_height/2
            
            #x轴方向速度的设置
            if bx < px:
                self.ball.velocity.x = -abs(self.ball.velocity.x)
            else:
                self.ball.velocity.x = abs(self.ball.velocity.x)

    ''' 检测小球与砖块的碰撞 '''
    def collision_ball_blocks(self):
        #检测碰撞的砖块
        hit_block = pygame.sprite.spritecollideany(self.ball, self.block_group)

        if hit_block != None:
            #碰撞则加分
            cfg.score += 10
            #移除碰撞到的砖块
            self.block_group.remove(hit_block)
            #小球中心点的坐标
            bx = self.ball.get_x() + self.ball.frame_width/2
            by = self.ball.get_y() + self.ball.frame_height/2
            
            #碰撞后小球的速度方向的设置
            if bx > hit_block.get_x() + self.ball.frame_width/2 and \
            bx < hit_block.get_x() + hit_block.frame_width - self.ball.frame_width/2:
                if by < hit_block.get_y() + hit_block.frame_height:
                    self.ball.velocity.y = -abs(self.ball.velocity.y)
                else:
                    self.ball.velocity.y = abs(self.ball.velocity.y)
            elif bx < hit_block.get_x() + self.ball.frame_width/2:
                self.ball.velocity.x = -abs(self.ball.velocity.x)
            elif bx > hit_block.get_x() + hit_block.frame_width - self.ball.frame_width/2:
                self.ball.velocity.x = abs(self.ball.velocity.x)
            else:
                self.ball.velocity.y *= -1
