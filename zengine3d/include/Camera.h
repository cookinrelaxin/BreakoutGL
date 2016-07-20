#ifndef CAMERA
#define CAMERA value

#include <vector>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    UP,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

enum class CameraStyle {
    NOCLIP,
    RTS,
    FPS,
};

const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 1.0f;
const GLfloat SENSITIVITY = .25f;
const GLfloat ZOOM = 45.0f;

class Camera {
    public:
        Camera(CameraStyle style,
               glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
               glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
               GLfloat yaw = YAW,
               GLfloat pitch = PITCH)
            : Front(glm::vec3(0.0f, 0.0f, -1.0f))
            , Style(style)
            , MovementSpeed(SPEED)
            , MouseSensitivity(SENSITIVITY)
            , Zoom(ZOOM)
            , Position(position)
            , WorldUp(up)
            , Yaw(yaw)
            , Pitch(pitch) {
                this->updateCameraVectors();
            }

        Camera(CameraStyle style,
               GLfloat posX,
               GLfloat posY,
               GLfloat posZ,
               GLfloat upX,
               GLfloat upY,
               GLfloat upZ,
               GLfloat yaw,
               GLfloat pitch = PITCH)
            : Front(glm::vec3(0.0f, 0.0f, -1.0f))
            , Style(style)
            , MovementSpeed(SPEED)
            , MouseSensitivity(SENSITIVITY)
            , Zoom(ZOOM)
            , Position(glm::vec3(posX, posY, posZ))
            , WorldUp(glm::vec3(upX, upY, upZ))
            , Yaw(yaw)
            , Pitch(pitch) {
                this->updateCameraVectors();
            }

        glm::mat4 GetViewMatrix() {
           return glm::lookAt(this->Position, this->Position + this->Front, this->Up); 
        }

        void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime) {
            GLfloat velocity = this->MovementSpeed * deltaTime;
            if (Style == CameraStyle::RTS) {
                // if (direction == UP)        this->Position += this->Up * velocity;
                if (direction == FORWARD)   this->Position.z += velocity;
                if (direction == BACKWARD)  this->Position.z -= velocity;
                if (direction == LEFT)      this->Position.x += velocity;
                if (direction == RIGHT)     this->Position.x -= velocity;
                // if (direction == RIGHT)     this->Position += this->Right * velocity;
            }
            else {
                if (direction == UP)        this->Position += this->Up * velocity;
                if (direction == FORWARD)   this->Position += this->Front * velocity;
                if (direction == BACKWARD)  this->Position -= this->Front * velocity;
                if (direction == LEFT)      this->Position -= this->Right * velocity;
                if (direction == RIGHT)     this->Position += this->Right * velocity;
            }
        }

        void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true) {
           xoffset *= this->MouseSensitivity; 
           yoffset *= this->MouseSensitivity; 

           this->Yaw += xoffset;
           this->Pitch += yoffset;

           if (constrainPitch) {
               if (this->Pitch > 89.0f) this->Pitch = 89.0f;
               if (this->Pitch < -89.0f) this->Pitch = -89.0f;
           }

           this->updateCameraVectors();
        }

        void ProcessMouseScroll(GLfloat yoffset) {
            if (this->Zoom >= 1.0f && this->Zoom <= 45.0f) this->Zoom -= yoffset;
            if (this->Zoom <= 1.0f) this->Zoom = 1.0f;
            if (this->Zoom >= 45.0f) this->Zoom = 45.0f;
            //if (this->Zoom >= 44.5f && this->Zoom <= 45.0f) this->Zoom -= yoffset;
            //if (this->Zoom <= 44.5f) this->Zoom = 44.5f;
            //if (this->Zoom >= 45.0f) this->Zoom = 45.0f;
        }

        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;

        GLfloat Yaw;
        GLfloat Pitch;

        GLfloat MovementSpeed;
        GLfloat MouseSensitivity;
        GLfloat Zoom;

        CameraStyle Style;

    private:
        void updateCameraVectors() {
            glm::vec3 front;
            front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
            front.y = sin(glm::radians(this->Pitch));
            front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));

            this->Front = glm::normalize(front);
            this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
            this->Up =    glm::normalize(glm::cross(this->Right, this->Front));
        }
};

#endif /* ifndef CAMERA */
