var n = new ZEngine.SceneNode();
var paddle = new ZEngine.SpriteNode();

var MOVE_LEFT = false;
var MOVE_RIGHT = false;

var MOVEMENT_SPEED = 20;
function init() {
    n.backgroundColor = ZEngine.Vec4(0.25, 0.1, 0.5, 1.0);
    n.size = ZEngine.Vec2(1024, 768);
    n.name = "MyGame";

    paddle.size = ZEngine.Vec2(120, 30);
    paddle.position = ZEngine.Vec2((n.size.x / 2) - paddle.size.x / 2,
                                      n.size.y - paddle.size.y);
    paddle.zPosition = 1;
    paddle.color = ZEngine.Vec4(1.0, 1.0, 1.0, 1.0);
    paddle.texture = "./textures/paddle.png";

    n.addChild(paddle);

    var background = new ZEngine.SpriteNode();
    background.size = n.size;
    background.position = n.position;
    background.color = ZEngine.Vec4(1.0, 1.0, 1.0, 1.0);
    background.texture = "./textures/background.jpg";
    n.addChild(background);

    return n;
}

function update(dt) {
    if (MOVE_LEFT) {
        paddle.position.x -= MOVEMENT_SPEED;
        // if (paddle.position.x 
    }
    if (MOVE_RIGHT)
        paddle.position.x += MOVEMENT_SPEED;

    return true;
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
