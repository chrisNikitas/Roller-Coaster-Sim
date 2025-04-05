int init(GLFWwindow*& window, bool& retflag);

void printWelcomeMessage();

void drawScene(Shader& skyboxShaderProgram, VertexBuffer& skyboxVBO, VertexArray& skyboxVAO, unsigned int sky, const Shader& gizmosShaderProgram, Shader& splinesShaderProgram, Shader& trackShaderProgram, Shader& primitivesShaderProgram);

void drawGround(Shader& groundShaderProgram, const Shader& gizmosShaderProgram, glm::mat4 lightProjectionViewMatrix, int depthMap, int groundTexture, int groundTextureNorm);

void drawSkybox(Shader& skyboxShaderProgram, VertexBuffer& skyboxVBO, VertexArray& skyboxVAO, unsigned int sky);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void createSM(const unsigned int& SHADOW_WIDTH, const unsigned int& SHADOW_HEIGHT, unsigned int depthMapFBO, const Shader& shaderProgram, const Shader& gizmosShaderProgram, const glm::mat4& lightProjectionViewMatrix, int depthMap);

void loadTexture(unsigned int& groundTexture, int& width, int& height, string path);

void loadSkyboxTextures(std::string& path, unsigned char*& data, int& width, int& height, int& nrComponents);

void createDepthMapTexture(unsigned int& depthMap, const unsigned int& SHADOW_WIDTH, const unsigned int& SHADOW_HEIGHT, unsigned int depthMapFBO);

void drawTrack(Shader& trackShaderProgram, const glm::mat4& lightProjectionViewMatrix, const Shader& splinesShaderProgram, const Shader& gizmosShaderProgram);

void error_callback(int error, const char* description);

void APIENTRY glDebugOutput(GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei length,
    const char* message,
    const void* userParam);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
