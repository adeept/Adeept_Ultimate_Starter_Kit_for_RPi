var canvas = document.getElementById("myCanvas");
var ctx = canvas.getContext("2d");
var w = 15;
var snaLen = 6;
var snake = [];
for (var i = 0; i < snaLen; i++) {
    snake[i] = new cell(i, 0, 39);
}
var head = snake[snaLen - 1]; 

var foodx = Math.ceil(Math.random() * 28 + 1);
var foody = Math.ceil(Math.random() * 28 + 1);
var food = new Food(foodx, foody);

function Food(x, y) {
    this.x = x;
    this.y = y;
    return this;
}

function cell(x, y, d) {
    this.x = x;
    this.y = y;
    this.d = d;
    return this;
}

function draw() {
    ctx.clearRect(0, 0, 450, 450);
    // for(var i = 0; i < 30; i++){
    //     ctx.strokeStyle = "#ccc";
    //     ctx.beginPath();
    //     ctx.moveTo(0,i*w);
    //     ctx.lineTo(450,i*w);

    //     ctx.moveTo(i*w,0);
    //     ctx.lineTo(i*w,450);
    //     ctx.closePath();
    //     ctx.stroke();
    // }
    for (var j = 0; j < snake.length; j++) {
        ctx.fillStyle = "#4B0082";
        if (j == snake.length - 1) {
            ctx.fillStyle = "#8A2BE2";
        }
        ctx.beginPath();
        ctx.rect(snake[j].x * w, snake[j].y * w, w, w);
        ctx.closePath();
        ctx.fill();
        ctx.stroke();
    }
    drawFood();
    if (head.x == food.x && head.y == food.y) {
        initFood();
        food = new Food(foodx, foody);
        drawFood();

        var newCell = new cell(head.x, head.y, head.d);
        switch (head.d) {
            case 40:
                newCell.y++;
                break; //下
            case 39:
                newCell.x++;
                break; //右
            case 38:
                newCell.y--;
                break; //上
            case 37:
                newCell.x--;
                break; //左
        }
        snake[snake.length] = newCell;
        head = newCell;
        //head = 
    }
}
//随机初始化食物
function initFood() {
    foodx = Math.ceil(Math.random() * 28 + 1);
    foody = Math.ceil(Math.random() * 28 + 1);
    for (var i = 0; i < snake.length; i++) {
        if (snake[i].x == foodx && snake[i].y == foody) {
            initFood();
        }
    }
}
//画食物
function drawFood() {
    //绘制食物
    ctx.fillStyle = "#006400";
    ctx.beginPath();
    ctx.rect(food.x * w, food.y * w, w, w);
    ctx.closePath();
    ctx.fill();
}
draw();


//控制蛇移动方向
function moveSnake(keyCode) {
    var newSnake = [];
    var newCell = new cell(head.x, head.y, head.d); //头
    //身体
    for (var i = 1; i < snake.length; i++) {
        newSnake[i - 1] = snake[i];
    }
    newSnake[snake.length - 1] = newCell;
    newCell.d = keyCode;
    switch (keyCode) {
        case 40:
            newCell.y++;
            break; //下
        case 39:
            newCell.x++;
            break; //右
        case 38:
            newCell.y--;
            break; //上
        case 37:
            newCell.x--;
            break; //左
    }
    snake = newSnake;
    head = snake[snake.length - 1];
    checkDeath();
    draw();
}
//游戏规则
function checkDeath() {
    let flag = false
    //超出边框
    if (head.x >= 30 || head.y >= 30 || head.x < 0 || head.y < 0) {
        flag = true
    }
    //咬到自己
    for (var i = 0; i < snake.length - 1; i++) {
      if (head.x == snake[i].x && head.y == snake[i].y) {
        flag = true
      }
    }

    if (flag) {
        alert('Game Over!!!')
        clearInterval(moveInterval)
        window.location.reaload()
    }
}
//蛇自动走
function moveClock() {
    moveSnake(head.d);
}
var isMove = false;
function beginGame() {
    if (!isMove) {
        var moveInterval = setInterval(moveClock, 300)
        isMove = true
    }
}