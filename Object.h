#pragma once
#define OBJECT_H

class Object {
protected:
    vec3 position;
    vec3 rotation;
    vec3 scale;
    Object* parent;
    std::vector<Object*> children;

public:
    Object() : position(0.0f), rotation(0.0f), scale(1.0f), parent(nullptr) {}
    virtual ~Object() {
		for (Object* child : children) {
			delete child;
		}
    }

	vec3 getPosition() const { return position; }
	void setPosition(vec3 pos) { 
		position = pos;
		for (Object* child : children) {
			child->setPosition(child->getPosition() + pos);
		}
    }

	void addChild(Object* child) {
		children.push_back(child);
		child->parent = this;
	}

	void removeChild(Object* child) {
		for (int i = 0; i < children.size(); i++) {
			if (children[i] == child) {
				children.erase(children.begin() + i);
				child->parent = nullptr;
				break;
			}
		}
	}

	Object* getParent() { return parent; }
	std::vector<Object*> getChildren() { return children; }
};

class CameraObject : public Object {
public:
	Camera* camera;
	float cameraSpeed = 0.05f;
	float cameraSpeedUp = 0.05f;
	float cameraSpeedDown = 0.05f;

	CameraObject(Camera* cam) : Object() {
		camera = cam;
	}

	static CameraObject* create(Camera* c) {
		return new CameraObject(c);
	}

	void update(GLFWwindow* window) {
		keyboardInput(window);
		mouseInput(window);
	}

private:
	void keyboardInput(GLFWwindow* window) {
		vec3 frontWithoutUp = vec3(camera->front.x, 0, camera->front.z);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera->pos += cameraSpeed * frontWithoutUp;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera->pos -= cameraSpeed * frontWithoutUp;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera->pos -= glm::normalize(glm::cross(frontWithoutUp, camera->up)) * cameraSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera->pos += glm::normalize(glm::cross(frontWithoutUp, camera->up)) * cameraSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			camera->pos += cameraSpeedUp * camera->up;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			camera->pos -= cameraSpeedDown * camera->up;
		}
		setPosition(camera->pos);
	}

	void mouseInput(GLFWwindow* window) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		if (!(xpos < 0 || xpos > WIDTH || ypos < 0 || ypos > HEIGHT)) {
			glfwSetCursorPos(window, static_cast<float>(WIDTH) / 2, static_cast<float>(HEIGHT) / 2);

			float xoffset = xpos - WIDTH / 2;
			float yoffset = HEIGHT / 2 - ypos;
			float sensitivity = 0.1f;
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			camera->yaw += xoffset;
			camera->pitch += yoffset;

			updateCameraVectors();
		}
	}

	void updateCameraVectors() {
		vec3 direction = vec3(0);
		direction.x = cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
		direction.y = sin(glm::radians(camera->pitch));
		direction.z = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
		camera->front = glm::normalize(direction);
	}
};