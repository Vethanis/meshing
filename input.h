#ifndef INPUT_H
#define INPUT_H

class Camera;
struct GLFWwindow;

class Input{
    GLFWwindow* m_glwindow;

    static bool m_rightMouseDown, m_leftMouseDown;
    static float m_scrollOffset, m_relScroll, m_cursorX, m_cursorY, m_relCursorX, m_relCursorY;

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

public:
    Input(GLFWwindow* window);
    void poll();
    void poll(float dt, Camera& cam);
    static bool rightMouseDown();
    static bool leftMouseDown();
    static float scrollOffset();
    static float relScroll();
    static float cursorX();
    static float cursorY();
    static float relCursorX();
    static float relCursorY();
    static int activeKey();
};
#endif
