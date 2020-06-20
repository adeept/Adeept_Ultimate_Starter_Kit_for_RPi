"""""""""""""""""""""""
""""""""主函数"""""""""
""""""""""""""""""""""""

import sys
import time
import datetime
import random
import math
import pygame
from pygame.locals import *
from Sprite import *
import setting as cfg
from Game import *
import Joystick # use Joystick

#初始化游戏并加载砖块
game=Game()
game.game_init()
game.load_level()

#实时监听
while True:

    #更新时钟，指定循环频率
    game.timer.tick(30)

    #获取自pygame.init（）调用以来的毫秒数
    ticks = pygame.time.get_ticks()

    #监听用户事件
    for event in pygame.event.get():
        # 判断用户是否点击了关闭按钮
        if event.type == QUIT:
            print("Game exit...")
            pygame.quit()  # 卸载所有pygame模块
            exit()
        #键盘被放开
        if event.type == KEYUP:
            if event.key == K_RETURN:
                game.goto_next_level()
    
    #监测键盘
    keys = pygame.key.get_pressed()

    #退出游戏
    if keys[K_ESCAPE]:
        sys.exit()

    #游戏未结束
    if not cfg.game_over:
        # Joystick Controller
        Joystick.setup()
        joystick = Joystick.direction()

        #游戏相关操作与检查碰撞
        game.update_blocks(ticks)
        game.move_paddle(joystick,ticks)
        game.move_ball(ticks)
        game.collision_ball_paddle()
        game.collision_ball_blocks()
        
        #用蓝色填充窗口
        game.screen.fill((50,50,100))

        #绘制精灵组
        game.block_group.draw(game.screen)
        game.ball_group.draw(game.screen)
        game.paddle_group.draw(game.screen)
        
        #打印相关数据
        print_text(game.font, 10, 0, "SCORE "+str(cfg.score))
        print_text(game.font, 170, 0, "LEVEL "+str(cfg.level+1))
        print_text(game.font, 320, 0, "BLOCKS "+str(len(game.block_group)))
        print_text(game.font, 490, 0, "BALLS "+str(cfg.lives))

    #游戏结束
    if cfg.game_over:
        print_text(game.font, 120, 380, "GAME OVER,YOU SCORE IS: "+str(cfg.score))

    #更新视图
    pygame.display.update()
    
    #线程睡眠
    time.sleep(1.0/60)
