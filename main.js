//EQUALITY MAY CAPTURE BY REFERENCE.
//you have been warned;

//TODO: fix 'Assertion failed: (objects_.find(object) != objects_.end() && "no object"), function remove_object, file ./includes/v8pp/class.hpp, line 100.' bug
//potential solution: keep heap allocated member variables for each member in a class
//
//js `new` is supposed to be used (by v8pp) to construct objects

//TODO: prevent y velocity from dropping to zero

var levels = require("./levels.js");
var LEVEL = levels.levels[0];
var blocks = [];

var n = new ZEngine.SceneNode();

var MOVE_LEFT = false;
var MOVE_RIGHT = false;

var BALL_STUCK = true;

var MOVEMENT_SPEED = 900;

var ball = new ZEngine.SpriteNode();
var paddle = new ZEngine.SpriteNode();
function init() {
    n.backgroundColor = new ZEngine.Vec4(0.25, 0.1, 0.5, 1.0);
    n.size = new ZEngine.Vec2(1024, 768);
    n.name = "Breakout!";

    levels.load_blocks(n, LEVEL);
    levels.load_ball(n, LEVEL, ball, paddle);

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
    if (ball.position.y <= 0) {
        ball.velocity.y = -ball.velocity.y;
        ball.position.y = 0.0;
    }
    if (ball.position.y >= n.size.y) {
        BALL_STUCK = true;
        ball.position.x = paddle.position.x + (paddle.size.x / 2) - (ball.size.x / 2);
        ball.position.y = paddle.position.y - ball.size.y;
        ball.velocity = new ZEngine.Vec2(LEVEL.ball.initial_velocity.x,
                                         LEVEL.ball.initial_velocity.y);
        reset_level();
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
        }
    }

    let c = ball.getCollision(paddle);
    if (c.exists) {
        let centerBoard = paddle.position.x + paddle.size.x / 2;
        let distance = (ball.position.x + (ball.size.x / 2)) - centerBoard;
        let percentage = distance / (paddle.size.x / 2);
        let strength = 2.0;
        let oldVelocity = new ZEngine.Vec2(ball.velocity.x, ball.velocity.y);
        ball.velocity.x = LEVEL.ball.initial_velocity.x * percentage * strength;
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
    return new ZEngine.Vec2(v.x/l, v.y/l);
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

function reset_level() {
    for (const block of blocks) {
       block.destroyed = false;
       block.zPosition = 2;
    }
}
