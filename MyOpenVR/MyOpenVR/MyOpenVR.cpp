#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#if defined( OSX )
#include <Foundation/Foundation.h>
#include <AppKit/AppKit.h>
#include <OpenGL/glu.h>
// Apple's version of glut.h #undef's APIENTRY, redefine it
#define APIENTRY
#else
#include <GL/glu.h>
#endif
#include <stdio.h>
#include <string>
#include <cstdlib>

#include <openvr.h>

#include "shared/lodepng.h"
#include "shared/Matrices.h"
#include "shared/pathtools.h"

#if defined(POSIX)
#include "unistd.h"
#endif

#ifndef _WIN32
#define APIENTRY
#endif

#ifndef _countof
#define _countof(x) (sizeof(x)/sizeof((x)[0]))
#endif


class CGLRenderModel
{
public:
	CGLRenderModel(const std::string & sRenderModelName);
	~CGLRenderModel();

	bool BInit(const vr::RenderModel_t & vrModel, const vr::RenderModel_TextureMap_t & vrDiffuseTexture);
	void Cleanup();
	void Draw();
	const std::string & GetName() const { return m_sModelName; }

private:
	GLuint m_glVertBuffer;
	GLuint m_glIndexBuffer;
	GLuint m_glVertArray;
	GLuint m_glTexture;
	GLsizei m_unVertexCount;
	std::string m_sModelName;
};

class CMainApplication
{
public:
	CMainApplication();
	virtual ~CMainApplication();

	bool BInit();
	bool BInitGL();
	bool BInitCompositor();

	void SetupRenderModels();

	void Shutdown();

	void RunMainLoop();
	bool HandleInput();
	void ProcessVREvent(const vr::VREvent_t & event);
	void RenderFrame();

	bool SetupTexturemaps();

	void SetupScene();
	void AddCubeToScene(Matrix4 mat, std::vector<float> &vertdata);
	void AddCubeVertex(float fl0, float fl1, float fl2, float fl3, float fl4, std::vector<float> &vertdata);

	void RenderControllerAxes();

	bool SetupStereoRenderTargets();
	void SetupCompanionWindow();
	void SetupCameras();

	void RenderStereoTargets();
	void RenderCompanionWindow();
	void RenderScene(vr::Hmd_Eye nEye);

	Matrix4 GetHMDMatrixProjectionEye(vr::Hmd_Eye nEye);
	Matrix4 GetHMDMatrixPoseEye(vr::Hmd_Eye nEye);
	Matrix4 GetCurrentViewProjectionMatrix(vr::Hmd_Eye nEye);
	void UpdateHMDMatrixPose();

	Matrix4 ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose);

	GLuint CompileGLShader(const char *pchShaderName, const char *pchVertexShader, const char *pchFragmentShader);
	bool CreateAllShaders();

	void SetupRenderModelForTrackedDevice(vr::TrackedDeviceIndex_t unTrackedDeviceIndex);
	CGLRenderModel *FindOrLoadRenderModel(const char *pchRenderModelName);

private:
	bool m_bDebugOpenGL;
	bool m_bVerbose;
	bool m_bPerf;
	bool m_bVblank;
	bool m_bGlFinishHack;

	vr::IVRSystem *m_pHMD;
	vr::IVRRenderModels *m_pRenderModels;
	std::string m_strDriver;
	std::string m_strDisplay;
	vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	Matrix4 m_rmat4DevicePose[vr::k_unMaxTrackedDeviceCount];
	bool m_rbShowTrackedDevice[vr::k_unMaxTrackedDeviceCount];

private: // SDL bookkeeping
	SDL_Window *m_pCompanionWindow;
	uint32_t mWindowWidth;
	uint32_t mWindowHeight;

	SDL_GLContext m_pContext;

private: // OpenGL bookkeeping
	int m_iTrackedControllerCount;
	int m_iTrackedControllerCount_Last;
	int m_iValidPoseCount;
	int m_iValidPoseCount_Last;
	bool m_bShowCubes;

	std::string m_strPoseClasses;                            // what classes we saw poses for this frame
	char m_rDevClassChar[vr::k_unMaxTrackedDeviceCount];   // for each device, a character representing its class

	int m_iSceneVolumeWidth;
	int m_iSceneVolumeHeight;
	int m_iSceneVolumeDepth;
	float m_fScaleSpacing;
	float m_fScale;

	int m_iSceneVolumeInit;                                  // if you want something other than the default 20x20x20

	float m_fNearClip;
	float m_fFarClip;

	GLuint m_iTexture;

	unsigned int m_uiVertcount;

	GLuint m_glSceneVertBuffer;
	GLuint m_unSceneVAO;
	GLuint m_unCompanionWindowVAO;
	GLuint m_glCompanionWindowIDVertBuffer;
	GLuint m_glCompanionWindowIDIndexBuffer;
	unsigned int m_uiCompanionWindowIndexSize;

	GLuint m_glControllerVertBuffer;
	GLuint m_unControllerVAO;
	unsigned int m_uiControllerVertcount;

	Matrix4 m_mat4HMDPose;
	Matrix4 m_mat4eyePosLeft;
	Matrix4 m_mat4eyePosRight;

	Matrix4 m_mat4ProjectionCenter;
	Matrix4 m_mat4ProjectionLeft;
	Matrix4 m_mat4ProjectionRight;

	struct VertexDataScene
	{
		Vector3 position;
		Vector2 texCoord;
	};

	struct VertexDataWindow
	{
		Vector2 position;
		Vector2 texCoord;

		VertexDataWindow(const Vector2 & pos, const Vector2 tex) : position(pos), texCoord(tex) {	}
	};

	GLuint m_unSceneProgramID;
	GLuint m_unCompanionWindowProgramID;
	GLuint m_unControllerTransformProgramID;
	GLuint m_unRenderModelProgramID;

	GLint m_nSceneMatrixLocation;
	GLint m_nControllerMatrixLocation;
	GLint m_nRenderModelMatrixLocation;

	struct FramebufferDesc
	{
		GLuint m_nDepthBufferId;
		GLuint m_nRenderTextureId;
		GLuint m_nRenderFramebufferId;
		GLuint m_nResolveTextureId;
		GLuint m_nResolveFramebufferId;
	};
	FramebufferDesc leftEyeDesc;
	FramebufferDesc rightEyeDesc;

	bool CreateFrameBuffer(int nWidth, int nHeight, FramebufferDesc &framebufferDesc);

	uint32_t m_nRenderWidth;
	uint32_t m_nRenderHeight;

	std::vector< CGLRenderModel * > m_vecRenderModels;
	CGLRenderModel *m_rTrackedDeviceToRenderModel[vr::k_unMaxTrackedDeviceCount];
};


CMainApplication::CMainApplication() 
: m_pCompanionWindow(NULL)
, m_pContext(NULL)
, mWindowWidth(1280)
, mWindowHeight(640)
, m_unSceneProgramID(0)
, m_unCompanionWindowProgramID(0)
, m_unControllerTransformProgramID(0)
, m_unRenderModelProgramID(0)
, m_pHMD(NULL)
, m_pRenderModels(NULL)
, m_bDebugOpenGL(false)
, m_bVerbose(false)
, m_bPerf(false)
, m_bVblank(false)
, m_bGlFinishHack(true)
, m_glControllerVertBuffer(0)
, m_unControllerVAO(0)
, m_unSceneVAO(0)
, m_nSceneMatrixLocation(-1)
, m_nControllerMatrixLocation(-1)
, m_nRenderModelMatrixLocation(-1)
, m_iTrackedControllerCount(0)
, m_iTrackedControllerCount_Last(-1)
, m_iValidPoseCount(0)
, m_iValidPoseCount_Last(-1)
, m_iSceneVolumeInit(20)
, m_strPoseClasses("")
, m_bShowCubes(true)
{}

CMainApplication::~CMainApplication()
{
	// work is done in Shutdown
	printf("Shutdown");
}

std::string GetTrackedDeviceString(vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL)
{
	uint32_t unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, NULL, 0, peError);
	if (unRequiredBufferLen == 0)
		return "";

	char *pchBuffer = new char[unRequiredBufferLen];
	unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, unRequiredBufferLen, peError);
	std::string sResult = pchBuffer;
	delete[] pchBuffer;
	return sResult;
}

bool CMainApplication::BInit()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		printf("%s - SDL could not initialize! SDL Error: %s\n", __FUNCTION__, SDL_GetError());
		return false;
	}

	// Loading the SteamVR Runtime
	vr::EVRInitError eError = vr::VRInitError_None;
	m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);

	if (eError != vr::VRInitError_None)
	{
		m_pHMD = NULL;
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "VR_Init Failed", buf, NULL);
		return false;
	}


	m_pRenderModels = (vr::IVRRenderModels *)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &eError);
	if (!m_pRenderModels)
	{
		m_pHMD = NULL;
		vr::VR_Shutdown();

		char buf[1024];
		sprintf_s(buf, sizeof(buf), "Unable to get render model interface: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "VR_Init Failed", buf, NULL);
		return false;
	}

	int nWindowPosX = 500;
	int nWindowPosY = 300;
	Uint32 unWindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	//SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY );
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
	if (m_bDebugOpenGL)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	m_pCompanionWindow = SDL_CreateWindow("MyOpenVR_RC", nWindowPosX, nWindowPosY, mWindowWidth, mWindowHeight, unWindowFlags);
	if (m_pCompanionWindow == NULL)
	{
		printf("%s - Window could not be created! SDL Error: %s\n", __FUNCTION__, SDL_GetError());
		return false;
	}

	m_pContext = SDL_GL_CreateContext(m_pCompanionWindow);
	if (m_pContext == NULL)
	{
		printf("%s - OpenGL context could not be created! SDL Error: %s\n", __FUNCTION__, SDL_GetError());
		return false;
	}

	glewExperimental = GL_TRUE;
	GLenum nGlewError = glewInit();
	if (nGlewError != GLEW_OK)
	{
		printf("%s - Error initializing GLEW! %s\n", __FUNCTION__, glewGetErrorString(nGlewError));
		return false;
	}
	glGetError(); // to clear the error caused deep in GLEW

	if (SDL_GL_SetSwapInterval(m_bVblank ? 1 : 0) < 0)
	{
		printf("%s - Warning: Unable to set VSync! SDL Error: %s\n", __FUNCTION__, SDL_GetError());
		return false;
	}

	if (!BInitGL())
	{
		printf("%s - Unable to initialize OpenGL!\n", __FUNCTION__);
		return false;
	}

	return true;
}

void CMainApplication::RunMainLoop()
{
	bool bQuit = false;
		
	while (!bQuit)
	{
		RenderFrame();
	}
}

bool CMainApplication::BInitGL()
{
	if (m_bDebugOpenGL)
	{
		//glDebugMessageCallback((GLDEBUGPROC)DebugCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}

	//SetupTexturemaps();
	SetupScene();
	SetupCameras();
	//SetupStereoRenderTargets();
	//SetupCompanionWindow();
	//SetupRenderModels();

	return true;
}

void CMainApplication::SetupScene()
{
	if (!m_pHMD)
		return;

	std::vector<float> vertdataarray;

	/*
	glGenVertexArrays(1, &m_unSceneVAO);
	glBindVertexArray(m_unSceneVAO);

	glGenBuffers(1, &m_glSceneVertBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_glSceneVertBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertdataarray.size(), &vertdataarray[0], GL_STATIC_DRAW);

	GLsizei stride = sizeof(VertexDataScene);
	uintptr_t offset = 0;

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void *)offset);

	offset += sizeof(Vector3);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void *)offset);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	*/

}

void CMainApplication::SetupCameras()
{
	/*
	m_mat4ProjectionLeft = GetHMDMatrixProjectionEye(vr::Eye_Left);
	m_mat4ProjectionRight = GetHMDMatrixProjectionEye(vr::Eye_Right);
	m_mat4eyePosLeft = GetHMDMatrixPoseEye(vr::Eye_Left);
	m_mat4eyePosRight = GetHMDMatrixPoseEye(vr::Eye_Right);
	*/
}

void CMainApplication::RenderFrame()
{
}

void CMainApplication::Shutdown()
{
}

int main(int argc, char *argv[])
{
	CMainApplication *pMainApplication = new CMainApplication();

	if (!pMainApplication->BInit())
	{
		pMainApplication->Shutdown();
		return 1;
	}

	pMainApplication->RunMainLoop();

	pMainApplication->Shutdown();

	return 0;
}