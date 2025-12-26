#include <iostream>
#include "OpenGLView.h"
#include "Application/Application.h"
#include "OpenGLBatchRender.h"
namespace OnYuu {

    double mousex, mousey;
    double xpos, ypos;


    static void cursor_position_callback(GLFWwindow* window, double mouse_x, double mouse_y) {

        // Visualizza le coordinate del mouse che si muove sulla finestra grafica
        OpenGLWindow* view = (OpenGLWindow*)glfwGetWindowUserPointer(window);
        mousex = mouse_x;
        mousey = mouse_y;
    }


    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        OpenGLWindow* view = (OpenGLWindow*)glfwGetWindowUserPointer(window);
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            //glfwGetCursorPos � un'utilit� fornita dalla libreria GLFW che permette di recuperare la posizione attuale del cursore
            //del mouse all'interno di una finestra OpenGL.
            glfwGetCursorPos(window, &xpos, &ypos);
        }
    }



    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        OpenGLWindow* view = (OpenGLWindow*)glfwGetWindowUserPointer(window);
        // Esegui le azioni desiderate in base alla quantit� di scorrimento
        std::cout << "Scorrimento x: " << xoffset << " y: " << yoffset << std::endl;
    }

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        OpenGLWindow* view = (OpenGLWindow*)glfwGetWindowUserPointer(window);
        // Se il tasto ESCAPE � premuto, chiude la finestra
        switch (key) {
        case GLFW_KEY_ESCAPE:
            if (action == GLFW_PRESS)
                //Imposta a True il flag booleano di chiusura della finestra
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;


        default:
            // codice da eseguire se expression non corrisponde a nessun valore
            break;
        }

    }

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        float w_update = 0;
        float h_update = 0;
        float x_offset = 0.0f, y_offset = 0.0f; // Offset per centrare la viewport

        // Prevenzione di divisione per zero
        if (height == 0) height = 1;

        // Ottieni l'istanza OpenGLWindow associata alla finestra GLFW
        OpenGLWindow* view = (OpenGLWindow*)glfwGetWindowUserPointer(window);
        if (!view) {
            // Se non abbiamo un puntatore valido, evitiamo di dereferenziare e usciamo
            return;
        }

        // Aggiorna la matrice di proiezione ortografica in base alle dimensioni del mondo virtuale

        // Calcola il rapporto di aspetto del "mondo" usando l'istanza view (evitiamo Application singleton)
        float AspectRatio_mondo = (float)view->getWidth() / (float)view->getHeight();

        // Adatta la viewport a seconda del rapporto tra il mondo e la finestra
        if (AspectRatio_mondo > (float)width / (float)height)
        {
            // Il mondo � relativamente pi� largo della finestra.
            // La larghezza della viewport � quella della finestra, l�altezza viene ridotta.
            w_update = (float)width;
            h_update = width / AspectRatio_mondo;
            // Calcola l�offset verticale per centrare la viewport nel framebuffer pi� alto
            y_offset = ((float)height - h_update) / 2.0f;
        }
        else
        {
            // Il mondo � relativamente pi� stretto o alto della finestra.
            // L�altezza della viewport � quella della finestra, la larghezza viene ridotta.
            h_update = (float)height;
            w_update = height * AspectRatio_mondo;
            // Calcola l�offset orizzontale per centrare la viewport nel framebuffer pi� largo
            x_offset = ((float)width - w_update) / 2.0f;
        }

        // Imposta la viewport OpenGL con offset e dimensioni calcolate per centrarla e mantenere il giusto aspect ratio
        glViewport((int)0, (int)0, (int)width, (int)height);

        // Aggiorna lo stato della finestra (evitiamo di chiamare Application singleton che potrebbe non essere inizializzato)
        view->resize((uint32_t)width, (uint32_t)height);
    }


    OpenGLWindow::OpenGLWindow(uint32_t width, uint32_t height)
        :Window(width, height)
    {
        /* Inizializza GLFW */
        if (!glfwInit())  // Se l’inizializzazione fallisce, esce dal programma con codice -1
            std::exit(-1);

        // Setta versioni OpenGL e profilo core (3.3 core profile)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Abilita il double buffering per rendering fluido
        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

        /* Crea la finestra con contesto OpenGL */
        window = glfwCreateWindow(1900, 1000, "Hello World", NULL, NULL);

        if (!window) // Controlla la creazione della finestra
        {
            std::cout << "Creazione Finestra Fallita!" << std::endl; // Se fallito, stampa errore
            glfwTerminate(); // Libera risorse GLFW
            std::exit(-1);  // Esce segnalando errore
        }

        /* Rende corrente il contesto OpenGL della finestra appena creata */
        glfwMakeContextCurrent(window);

        /* Carica i puntatori OpenGL tramite GLAD */
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Fallisce il caricamento dei puntatori alle  !" << std::endl;
            glfwTerminate(); // Libera risorse se fallisce
            std::exit(-1);
        }
        glfwSetWindowUserPointer(window, this);
        // Registra callback per eventi: tastiera, posizione mouse, click mouse, resize viewport
        glfwSetKeyCallback(window, key_callback);
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetWindowAspectRatio(window, 16, 9);
        glEnable(GL_DEPTH_TEST); // Abilita il depth test per il rendering 3D
    }

    OpenGLWindow::~OpenGLWindow()
    {
        glfwDestroyWindow(window);
        glfwTerminate();           // Termina GLFW e libera risorse residue

    }
    void OpenGLWindow::beginFrame()
    {

        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
    }
    void OpenGLWindow::draw()
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    double OpenGLWindow::getFrameTime()
    {

        return deltaTime;
    }

    bool OpenGLWindow::shouldClose()
    {
        return glfwWindowShouldClose(window);
    }
}