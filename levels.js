module.exports = {
    levels: [
        {
            name: "The first!",
            blocks: {
                list: [
                    [ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ],
                    [ 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4 ],
                    [ 4, 1, 4, 1, 4, 0, 0, 1, 0, 0, 4, 1, 4, 1, 4 ],
                    [ 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3 ],
                    [ 3, 3, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 3, 3 ],
                    [ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 ],
                    [ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 ]
                ],
                rgb_colors: {
                    "0": {
                        "red": 70,
                        "green": 137,
                        "blue": 102
                    },
                    "1": {
                        "red": 255,
                        "green": 240,
                        "blue": 165
                    },
                    "2": {
                        "red": 255,
                        "green": 176,
                        "blue": 59
                    },
                    "3": {
                        "red": 182,
                        "green": 73,
                        "blue": 38
                    },
                    "4": {
                        "red": 142,
                        "green": 40,
                        "blue": 0
                    }
                },
                "textures": {
                    "0": "./textures/block.png",
                    "1": "./textures/block.png",
                    "2": "./textures/block.png",
                    "3": "./textures/block.png",
                    "4": "./textures/block.png"
                }
            },
            ball: {
                color: {
                    "red": 360,
                    "green": 360,
                    "blue": 360
                },
                texture: "./textures/awesomeface.png",
                initial_velocity: {
                    "x": 500,
                    "y": 700
                },
                radius: 25
            },
        },
    ],
    load_blocks: load_blocks,
    load_ball: load_ball
};

function load(scene, level) {
    load_blocks(scene, level);
    load_ball(scene, level);
}

function load_ball(scene, level, ball, paddle) {
    const INITIAL_VELOCITY = new ZEngine.Vec2(500, 700);

    paddle.size = new ZEngine.Vec2(130, 30);
    paddle.position = new ZEngine.Vec2((n.size.x / 2) - paddle.size.x / 2,
                                      n.size.y - paddle.size.y);
    paddle.zPosition = 1;
    paddle.color = new ZEngine.Vec4(1.0, 1.0, 1.0, 1.0);
    paddle.texture = "./textures/paddle.png";

    scene.addChild(paddle);

    var background = new ZEngine.SpriteNode();
    background.size = scene.size;
    background.position = scene.position;
    background.color = new ZEngine.Vec4(123/360.0, 137/360.0, 130/360.0, 1.0);
    n.addChild(background);

    // ball.size = ZEngine.Vec2(50, 50);
    ball.size = new ZEngine.Vec2(level.ball.radius*2, level.ball.radius*2);
    let color = level.ball.color;
    ball.color = new ZEngine.Vec4(color["red"]/360.0,
                                  color["green"]/360.0,
                                  color["blue"]/360.0,
                                  1.0);
    ball.texture = level.ball.texture;
    ball.position.x = paddle.position.x + (paddle.size.x / 2) - (ball.size.x / 2);
    ball.position.y = paddle.position.y - ball.size.y;
    ball.zPosition = 3;

    ball.velocity = new ZEngine.Vec2(level.ball.initial_velocity.x,
                                     level.ball.initial_velocity.y);

    n.addChild(ball);
}

function load_blocks(scene, level) {
    let block_width = scene.size.x / level.blocks.list[0].length;
    let block_height = block_width / 1.3;
    let x = 0;
    let y = 0;
    for (const row of level.blocks.list) {
         x = 0;
        for (const col of row) {
            let block = new ZEngine.SpriteNode();
            block.size = new ZEngine.Vec2(block_width, block_height);
            block.position = ZEngine.Vec2(x, y);
            block.zPosition = 2;
            block.texture = level.blocks.textures[col];
            let colors = level.blocks.rgb_colors[col];
            block.color = new ZEngine.Vec4(colors.red/360.0,
                                       colors.green/360.0,
                                       colors.blue/360.0,
                                       1.0);
            scene.addChild(block);
            blocks.push(block);
            x += block_width;
        }
        y += block_height;
    }
}
