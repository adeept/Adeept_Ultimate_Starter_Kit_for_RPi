"""""""""""""""""""""""
""""""""精灵类"""""""""
""""""""""""""""""""""""

import sys
import pygame
import random
import math
import time
import datetime
from pygame.locals import *


'''精灵类'''
class Sprite(pygame.sprite.Sprite):

    '''初始化'''
    def __init__(self, target):
        pygame.sprite.Sprite.__init__(self)
        self.master_image = None
        self.old_frame = -1
        self.frame = 0
        self.frame_width = 1
        self.frame_height = 1
        self.first_frame = 0
        self.last_frame = 0
        self.columns = 1
        self.last_time = 0
        self.velocity = Point(0,0)

    '''获取矩形在x轴方向上的位置'''
    def get_x(self):
        return self.rect.x

    '''设置矩形在x轴方向上的位置'''
    def set_x(self, value):
        self.rect.x = value

    '''获取矩形在y轴方向上的位置'''
    def get_y(self):
        return self.rect.y

    '''设置矩形在y轴方向上的位置'''
    def set_y(self, value):
        self.rect.y = value

    '''
    增加矩形的x值 
    param {int} value 增加的值 向左运动- 向右运动+
    '''
    def add_x(self, value):
        self.rect.x += value

    '''
    增加矩形的y值
    param {int} value 增加的值 向上运动- 向下运动+
    '''
    def add_y(self, value):
        self.rect.y += value

    '''
    获取矩形距离屏幕左上的距离 
    return {tuple} (距离屏幕左边的距离,距离屏幕上边的距离) 
    '''
    def get_pos(self):
        return self.rect.topleft

    '''
    设置矩形的位置
    param {tuple} pos(距离屏幕左边的距离,距离屏幕上边的距离)
    '''
    def set_pos(self, pos):
        self.rect.topleft = pos

    '''加载图片'''
    def load(self, filename, width=0, height=0, columns=1):
        self.master_image = pygame.image.load(filename).convert_alpha()
        self.image = self.master_image
        self.set_image(self.master_image, width, height, columns)
        
    '''设置图片的相关参数'''
    def set_image(self, image, width=0, height=0, columns=1):
        self.master_image = image
        self.image = self.master_image
        if width == 0 and height == 0:
            self.frame_width = image.get_width()
            self.frame_height = image.get_height()
        else:
            self.frame_width = width
            self.frame_height = height
            rect = self.master_image.get_rect()
            self.first_frame = 1
            self.last_frame = (rect.width//width) * (rect.height//height) - 1
        self.rect = Rect(0,0,self.frame_width,self.frame_height)
        self.columns = columns
    
    '''更新动画帧'''
    def update(self, current_time, rate=30):
        if current_time > self.last_time + rate:
            self.frame += 1
            if self.frame > self.last_frame:
                self.frame = self.first_frame
            self.last_time = current_time
            if self.frame != self.old_frame:
                frame_x = (self.frame % self.columns) * self.frame_width
                frame_y = (self.frame // self.columns) * self.frame_height
                rect = Rect(frame_x, frame_y, self.frame_width, self.frame_height)
                self.image = self.master_image.subsurface(rect)
                self.old_frame = self.frame

    '''
    return {string} 返回拼接的字符串
    '''
    def __str__(self):
        return str(self.frame)+","+str(self.first_frame)+","+ \
               str(self.last_frame)+","+str(self.frame_width) + "," + \
               str(self.frame_height)+","+str(self.columns)+","+ str(self.rect)

'''打印text到屏幕上'''
def print_text(font, x, y, text, color=(255,255,255)):
    imageText = font.render(text, True, color)
    screen = pygame.display.get_surface()
    screen.blit(imageText, (x,y))

'''点类'''
class Point:
    def __init__(self, x, y):
        self.move(x, y)

    def move(self,x, y):
        self.x = x
        self.y = y

    def reset(self):
        self.move(0, 0)