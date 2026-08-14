var canvas = document.getElementById("myCanvas");
var ctx = canvas.getContext("2d");
var w = 15;
var snaLen = 6;
var snake = [];
var moveInterval = null;
var isMove = false;
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
}
function cell(x, y, d) {
    this.x = x;
    this.y = y;
    this.d = d;
}
function draw() {
    ctx.clearRect(0, 0, 450, 450);
    for (var j = 0; j < snake.length; j++) {
        ctx.fillStyle = j == snake.length - 1 ? "#8A2BE2" : "#4B0082";
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
        var newCell = new cell(head.x, head.y, head.d);
        switch (head.d) {
            case 40: newCell.y++; break;
            case 39: newCell.x++; break;
            case 38: newCell.y--; break;
            case 37: newCell.x--; break;
        }
        snake.push(newCell);
        head = newCell;
    }
}
function initFood() {
    foodx = Math.ceil(Math.random() * 28 + 1);
    foody = Math.ceil(Math.random() * 28 + 1);
    for (var i = 0; i < snake.length; i++) {
        if (snake[i].x == foodx && snake[i].y == foody) {
            initFood();
            return;
        }
    }
}
function drawFood() {
    ctx.fillStyle = "#006400";
    ctx.beginPath();
    ctx.rect(food.x * w, food.y * w, w, w);
    ctx.closePath();
    ctx.fill();
}
function moveSnake(keyCode) {
    if (keyCode == 32) {
        beginGame();
        return;
    }
    if (keyCode != 37 && keyCode != 38 && keyCode != 39 && keyCode != 40) {
        return;
    }
    var newSnake = [];
    var newCell = new cell(head.x, head.y, head.d);
    for (var i = 1; i < snake.length; i++) {
        newSnake[i - 1] = snake[i];
    }
    newSnake[snake.length - 1] = newCell;
    newCell.d = keyCode;
    switch (keyCode) {
        case 40: newCell.y++; break;
        case 39: newCell.x++; break;
        case 38: newCell.y--; break;
        case 37: newCell.x--; break;
    }
    snake = newSnake;
    head = snake[snake.length - 1];
    checkDeath();
    draw();
}
function checkDeath() {
    var flag = false;
    if (head.x >= 30 || head.y >= 30 || head.x < 0 || head.y < 0) {
        flag = true;
    }
    for (var i = 0; i < snake.length - 1; i++) {
        if (head.x == snake[i].x && head.y == snake[i].y) {
            flag = true;
            break;
        }
    }
    if (flag) {
        alert("Game Over!!!");
        if (moveInterval !== null) {
            clearInterval(moveInterval);
            moveInterval = null;
        }
        isMove = false;
        location.reload();
    }
}
function moveClock() {
    moveSnake(head.d);
}
function beginGame() {
    if (!isMove) {
        moveInterval = setInterval(moveClock, 300);
        isMove = true;
    }
}
draw();
