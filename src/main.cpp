#include <GL/glew.h>

#include <GL/freeglut.h>
#include <iostream>
#include <memory>
#include <unordered_map>

#include "scene.h"

static std::unique_ptr<ou::Scene> scene;

namespace ou {

class Callbacks {
public:
    static void timer(int)
    {
        glutPostRedisplay();
        glutTimerFunc(1000 / 60, timer, 0);
    }

    static void renderScene(void)
    {
        scene->render();
        glutSwapBuffers();
    }

    static void keyboardDown(unsigned char key, int, int)
    {
        scene->keyDown(key);
    }

    static void keyboardUp(unsigned char key, int, int)
    {
        scene->keyUp(key);
    }

    static void specialKeyboardDown(int key, int, int)
    {
        switch (key) {
        case GLUT_KEY_LEFT:
            scene->keyDown('a');
            break;
        case GLUT_KEY_RIGHT:
            scene->keyDown('d');
            break;
        case GLUT_KEY_UP:
            scene->keyDown('w');
            break;
        case GLUT_KEY_DOWN:
            scene->keyDown('s');
            break;
        }
    }

    static void specialKeyboardUp(int key, int, int)
    {
        switch (key) {
        case GLUT_KEY_LEFT:
            scene->keyUp('a');
            break;
        case GLUT_KEY_RIGHT:
            scene->keyUp('d');
            break;
        case GLUT_KEY_UP:
            scene->keyUp('w');
            break;
        case GLUT_KEY_DOWN:
            scene->keyUp('s');
            break;
        }
    }

    static void mouseMove(int x, int y)
    {
        scene->mouseMove(x, y);
    }

    static void mouseEvent(int button, int state, int, int)
    {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
            scene->mouseClick();
        }
    }

    static void mouseEntry(int state)
    {
        if (state == GLUT_ENTERED) {
            scene->mouseEnter();
        }
    }

    static void reshapeWindow(int width, int height)
    {
        scene->reshapeWindow(width, height);
    }

    static void GLAPIENTRY openglDebugCallback(GLenum source, GLenum type, GLenum id, GLenum severity,
        GLsizei length, const GLchar* message, const void* userParam)
    {
        const char* type_str;
        switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            type_str = "ERROR";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            type_str = "DEPRECATED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            type_str = "UNDEFINED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            type_str = "PORTABILITY";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            type_str = "PERFORMANCE";
            break;
        case GL_DEBUG_TYPE_MARKER:
            type_str = "MARKER";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            type_str = "PUSH_GROUP";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            type_str = "POP_GROUP";
            break;
        case GL_DEBUG_TYPE_OTHER:
            type_str = "OTHER";
            break;
        }

        const char* sev_str;
        switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            sev_str = "HIGH";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            sev_str = "MEDIUM";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            sev_str = "LOW";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            sev_str = "NOTIFICATION";
        }

        if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
            std::cout << "OpenGL Message: " << message
                      << " type=" << type_str
                      << " severity=" << sev_str
                      << "\n";

        } else {
            std::cerr << "OpenGL Message: " << message
                      << " type=" << type_str
                      << " severity=" << sev_str
                      << "\n";
        }
    }
};
}

int main(int argc, char* argv[])
{
    // init GLUT and create Window
    glutInit(&argc, argv);
    glutInitContextVersion(4, 5);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(600, 800);
    glutCreateWindow("Sogang CSE4170 20171634");
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error initializing GLEW\n";
        return 1;
    }

    // register callbacks
    glutKeyboardFunc(ou::Callbacks::keyboardDown);
    glutKeyboardUpFunc(ou::Callbacks::keyboardUp);
    glutSpecialFunc(ou::Callbacks::specialKeyboardDown);
    glutSpecialUpFunc(ou::Callbacks::specialKeyboardUp);
    glutDisplayFunc(ou::Callbacks::renderScene);
    glutPassiveMotionFunc(ou::Callbacks::mouseMove);
    glutEntryFunc(ou::Callbacks::mouseEntry);
    glutReshapeFunc(ou::Callbacks::reshapeWindow);
    glutMouseFunc(ou::Callbacks::mouseEvent);
    glutTimerFunc(0, ou::Callbacks::timer, 0);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(ou::Callbacks::openglDebugCallback, nullptr);

    try {
        scene = std::make_unique<ou::Scene>();

        // enter GLUT event processing cycle
        glutMainLoop();
    } catch (std::exception const& e) {
        std::cerr << "Exception caught while running program: " << e.what() << "\n";
    }

    glutExit();

	std::cout << "Exiting..." << std::endl;

    return 0;
}
