//EQUALITY MAY CAPTURE BY REFERENCE.
//you have been warned;
//
//TODO: fix 'Assertion failed: (objects_.find(object) != objects_.end() && "no object"), function remove_object, file ./includes/v8pp/class.hpp, line 100.' bug
//TODO: prevent y velocity from dropping to zero

var levels = require("./levels.js");
var blocks = [];

var n = new ZEngine.SceneNode();
var paddle = new ZEngine.SpriteNode();
var ball = new ZEngine.SpriteNode();

var MOVE_LEFT = false;
var MOVE_RIGHT = false;

var BALL_STUCK = true;
const INITIAL_VELOCITY = new ZEngine.Vec2(400, 600);

var MOVEMENT_SPEED = 900;
function init() {
    n.backgroundColor = ZEngine.Vec4(0.25, 0.1, 0.5, 1.0);
    n.size = ZEngine.Vec2(1024, 768);
    n.name = "Breakout!";

    paddle.size = ZEngine.Vec2(130, 30);
    paddle.position = ZEngine.Vec2((n.size.x / 2) - paddle.size.x / 2,
                                      n.size.y - paddle.size.y);
    paddle.zPosition = 1;
    paddle.color = ZEngine.Vec4(1.0, 1.0, 1.0, 1.0);
    paddle.texture = "./textures/paddle.png";

    n.addChild(paddle);

    var background = new ZEngine.SpriteNode();
    background.size = n.size;
    background.position = n.position;
    // background.color = ZEngine.Vec4(1.0, 1.0, 1.0, 1.0);
    background.color = ZEngine.Vec4(123/360.0, 137/360.0, 130/360.0, 1.0);
    // background.texture = "./textures/background.jpg";
    n.addChild(background);

    ball.size = ZEngine.Vec2(50, 50);
    ball.color = ZEngine.Vec4(1.0, 1.0, 1.0, 1.0);
    ball.texture = "./textures/awesomeface.png";
    ball.position.x = paddle.position.x + (paddle.size.x / 2) - (ball.size.x / 2);
    ball.position.y = paddle.position.y - ball.size.y;
    ball.zPosition = 3;

    ball.velocity = new ZEngine.Vec2(INITIAL_VELOCITY.x, INITIAL_VELOCITY.y);

    n.addChild(ball);

    load_level(n, levels.level_1);

    return n;
}

function update(dt) {
    let paddle_velocity = MOVEMENT_SPEED * dt;
    if (MOVE_LEFT) {
        paddle.position.x -= paddle_velocity;
        if (BALL_STUCK)
            ball.position.x -= paddle_velocity;
    }
    if (MOVE_RIGHT) {
        paddle.position.x += paddle_velocity;
        if (BALL_STUCK)
            ball.position.x += paddle_velocity;
    }
    if (!BALL_STUCK) {
        ball.position.x += ball.velocity.x * dt;
        ball.position.y += ball.velocity.y * dt;
    }

    if ((ball.position.x + ball.size.x) >= n.size.x) {
        ball.velocity.x = -ball.velocity.x;
        ball.position.x = n.size.x - ball.size.x;
    }
    else if ((ball.position.x) <= 0) {
        ball.velocity.x = -ball.velocity.x;
        ball.position.x = 0.0;
    }
    if ((ball.position.y) <= 0) {
        ball.velocity.y = -ball.velocity.y;
        ball.position.y = 0.0;
    }

    for (const block of blocks) {
        if (block.destroyed) continue;
        let c = ball.getCollision(block);
        if (c.exists) {
            block.destroyed = true;
            block.zPosition = -1;
            // block.color = ZEngine.Vec4(123/360.0, 137/360.0, 130/360.0, 1.0);
            if (c.direction === "LEFT" || c.direction === "RIGHT") {
                ball.velocity.x = -ball.velocity.x;
                var penetration = (ball.size.x / 2) - Math.abs(c.difference.x);
                if (c.direction === "LEFT")
                    ball.position.x += penetration;
                else
                    ball.position.x -= penetration;
            }
            else {
                ball.velocity.y = -ball.velocity.y;
                var penetration = (ball.size.y / 2) - Math.abs(c.difference.y);
                if (c.direction === "UP")
                    ball.position.y -= penetration;
                else
                    ball.position.y += penetration;
            }
            // n.removeChild(block);
            // blocks.pop(block);
        }
    }

    let c = ball.getCollision(paddle);
    if (c.exists) {
        // console.log("ball.velocity.x: ", ball.velocity.x);
        let centerBoard = paddle.position.x + paddle.size.x / 2;            //ok
        let distance = (ball.position.x + (ball.size.x / 2)) - centerBoard; //ok
        let percentage = distance / (paddle.size.x / 2);                    //ok
        let strength = 2.0;                                                 //ok
        let oldVelocity = ZEngine.Vec2(ball.velocity.x, ball.velocity.y);   //ok
        ball.velocity.x = INITIAL_VELOCITY.x * percentage * strength;       //ok
        ball.velocity.y = -1 * Math.abs(ball.velocity.y);
        let normalized = normalize_vec2(ball.velocity);
        let l = vec2_length(oldVelocity);
        ball.velocity.x = normalized.x * l;
        ball.velocity.y = normalized.y * l;

    }

    return true;
}

function vec2_length(v) {
    return Math.sqrt(v.x * v.x + v.y * v.y);
}

function normalize_vec2(v) {
    let l = vec2_length(v);
    return ZEngine.Vec2(v.x/l, v.y/l);
}

function shutdown() {
    return true;
}

function key_was_pressed(key) {
    switch (key) {
        case ZEngine.KEY_A:  {
            MOVE_LEFT = true;
            break;
        }
        case ZEngine.KEY_D:  {
            MOVE_RIGHT = true;
            break;
        }
        case ZEngine.KEY_SPACE:  {
            BALL_STUCK = false;
            break;
        }
    }
}

function key_was_released(key) {
    switch (key) {
        case ZEngine.KEY_A:  {
            MOVE_LEFT = false;
            break;
        }
        case ZEngine.KEY_D:  {
            MOVE_RIGHT = false;
            break;
        }
    }
}

function load_level(scene, level) {
    var block_width = scene.size.x / level[0].length;
    var block_height = block_width / 1.3;
    var x = 0;
    var y = 0;
    for (const row of level) {
        x = 0;
        for (const col of row) {
            var block = new ZEngine.SpriteNode();
            block.size = ZEngine.Vec2(block_width, block_height);
            block.position = ZEngine.Vec2(x, y);
            block.zPosition = 2;
            block.texture = "./textures/block.png";
            block.color = (function() {
                //firenze https://color.adobe.com/Firenze-color-theme-24198/?showPublished=true
                switch(col) {
                    case 0: {
                        return ZEngine.Vec4(70/360.0, 137/360.0, 102/360.0, 1.0);
                    }
                    case 1: {
                        return ZEngine.Vec4(255/360.0, 240/360.0, 165/360.0, 1.0);
                    }
                    case 2: {
                        return ZEngine.Vec4(255/360.0, 176/360.0, 59/360.0, 1.0);
                    }
                    case 3: {
                        return ZEngine.Vec4(182/360.0, 73/360.0, 38/360.0, 1.0);
                    }
                    case 4: {
                        return ZEngine.Vec4(142/360.0, 40/360.0, 0/360.0, 1.0);
                    }
                }
            })();
            scene.addChild(block);
            blocks.push(block);
            x += block_width;
        }
        y += block_height;
    }
}

