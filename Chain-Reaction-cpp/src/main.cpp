
#include "PrecompiledHeader.h" //precompiled header
#include "MainGame.h"
#include "AppGUIFrame.h"
#include "MeshGenerator.h"
#include "Text.h"
#include "SoundSystem.h"
#include "Utilities.h"

#if defined (_WIN32) //Include resource header only in Windows environment.
// On windows platform the icon is ported as a resource file and is not contained the zip file.
#include "Resource.h"
#else
// On unix platform the icon is included as an xpm resource file and is later used to create icon object with wxWidgets.
#include "icon.xpm" //This include file defines the 'chainReactionICON' variable
#endif //!_WIN32
/*
 * @brief A class derived from wxApp which serves as the main entry point of the wxWidget application.
 */
class wxAppInstance: public wxApp{
public:
	/*
	 * @brief Idle event handler of the application (wxAppInstance) which loads and extracts resources for further processing.
	 */
	bool loadZipResource();
	/*
	 * @brief IDLE event handler of the current application.
	 * @param {wxIdleEvent &} event: idle event object provided by the application object.
	 */
	 void OnIdle(wxIdleEvent &event);
	/*
	 * @brief Main entry point of the application (wxAppInstance).
	 */
	virtual bool OnInit();
	/*
	 * @brief Default destructor of application (wxAppInstance). Handles deletion of dynamically allocated memories.
	 */
	virtual int OnExit() wxOVERRIDE;
private:
	wxIcon _icon; //icon object that is later set as the icon property of the top window and the main frame.
	bool _isResourceExtracted = false;
	bool _isResourceProcessed = false;
	AppGUIFrame* _mainFrame = nullptr; //Top window of the current application which will later be created inside OnInit method.
	std::shared_ptr<MainGame> _offlineGameObject = nullptr; //Pointer to the object associated with an offline game.
	std::shared_ptr<GameUtilities::GameState> _gameInformation = nullptr; //Pointer to the object containing current game state.
	std::shared_ptr<ResourceUtilities::ResourceData> _resourceData = nullptr; //Pointer to resources containing e.g., text object created from alphabet mesh data, textures, etc.
};
//********************************************************* Source code ****************************
bool wxAppInstance::loadZipResource(){
	if (!this->_mainFrame) return false; //Top window is not yet created. Abort processing idle tasks.
	if (this->_isResourceProcessed) return true; //All idle tasks are taken care of.

	/*********************** preparation for loading and extracting resource files from a zip file *************************/
	wxLogDebug("[Chain-Reaction] Preparing for loading and extracting resource files from a zip file ...");
	//***** Set of file names that are contained in the zip resource. These file names are verified against the zip resource.
	std::set<std::string> fileList = { "A.obj", "B.obj", "C.obj", "D.obj",
									"E.obj", "F.obj", "G.obj", "H.obj",
									"I.obj", "J.obj", "K.obj", "L.obj",
									"M.obj", "N.obj", "O.obj", "P.obj",
									"Q.obj", "R.obj", "S.obj", "T.obj",
									"U.obj", "V.obj", "W.obj", "X.obj",
									"Y.obj", "Z.obj", "audio.wav", "fragment-shader-gridlines.txt",
									"vertex-shader-gridlines.txt", "vertex-shader-orbs.txt", "fragment-shader-orbs.txt", "vertex-shader-texts.txt",
									"fragment-shader-texts.txt", "vertex-shader-particles.txt", "fragment-shader-particles.txt", "alphabet.mtl" };
#if defined (_WIN32) 
	//Load icon and zip file from resource('.rc') file. Applicable to windows environment only.
	//************* Set icon property of the application ******************
	//On windows platform the icon is ported as a resource file and is not contained the zip file.
	//Locate and load icon file from the resource. Resource ID (IDI_ICON1) for the icon is defined in "resource.h" header.
	wxLogDebug(wxT("[Chain-Reaction] Loading icon and zip resources from memory ..."));
	HICON iconResource = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1));
	if (iconResource == NULL) {
		wxLogDebug(wxT("[Chain-Reaction] Error: failed to retrieve icon resource data from memory ..."));
		wxMessageBox(wxT("Failed to retrieve icon resource data from memory."), wxT("Resource Handling Error"), wxOK | wxICON_ERROR);
		return false;
	}
	this->_icon.CreateFromHICON(iconResource);
	//***** Locate the resource file (zip) in memory. Resource ID (IDR_ZIP1) for the zip is defined in "resource.h" header.
	HANDLE zip_resource_info = FindResource(GetModuleHandle(0), MAKEINTRESOURCE(IDR_ZIP1), TEXT("ZIP"));
	if (zip_resource_info == NULL) { //Handle error with locating zip resource
		wxLogDebug(wxT("[Chain-Reaction] Error: to retrieve zip resource information from memory ..."));
		wxMessageBox(wxT("Failed to retrieve zip resource information from memory."), wxT("Resource Handling Error"), wxOK | wxICON_ERROR);
		return false;
	}
	//***** Load the zip resource on heap.
	HANDLE zipResource = LoadResource(GetModuleHandle(0), (HRSRC)zip_resource_info);
	if (zipResource == NULL) { //Handle error with loading zip resource.
		wxMessageBox(wxT("Failed to retrieve resource data"), wxT("Resource Handling Error"), wxOK | wxICON_ERROR);
		//delete menu;
		return false;
	}
	//***** Retrive size of the zip resource.
	size_t zipSize = SizeofResource(GetModuleHandle(0), (HRSRC)zip_resource_info);
#endif //_WIN32
	unsigned int fileCount = fileList.size(); //dummy variable to keep track of progress and update progress dialog.
	int totalNumberOfTasks = 2 * (1 + fileCount) + 3;
	//*******************************************************************
	//**** Progress dialog to display progress on resource extraction, loading, initialization, etc.
	wxLogDebug("[Chain-Reaction] Extracting resources ...");
	shared_ptr<wxProgressDialog> progressDialog(new wxProgressDialog("Wait!", "Extracting resources ...", totalNumberOfTasks, this->_mainFrame, wxPD_AUTO_HIDE | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME));
	progressDialog->Update(1, "Extracting resources ...");
	std::vector<ResourceUtilities::FileData*> allResourceDataContainer;//Storage location for zip extraction. It is a vector containing the file data (e.g., file name, raw binary data, size of data) of each entry in the zip.
#if defined (_WIN32)
	//***** Extract zip file loaded from resource('.rc') file **
	this->_isResourceExtracted = ResourceUtilities::extractZipResource(zipResource, zipSize, allResourceDataContainer, fileList, progressDialog.get());
	FreeResource(zipResource); //Zip extraction completed. Free zip resource from heap memory.
#else
	wxLogDebug("[Chain-Reaction] Getting information on the location of the zip resource file ...");
	wxFileDialog resourceDialog(this->_mainFrame, "Choose resource zip file: ", wxGetCwd(), wxEmptyString, wxT("Resource files (*.zip)|*.zip"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	bool isDialogShown = resourceDialog.Show(true);
	if (isDialogShown) {
		int modalResult = resourceDialog.ShowModal();
		if (modalResult != wxID_CANCEL) {
			wxString pathToTheResource = resourceDialog.GetPath();
			wxLogDebug(wxString::Format("[Chain-Reaction] Extracting zip resource '%s' ...", pathToTheResource));
			this->_isResourceExtracted = ResourceUtilities::extractZipResource(pathToTheResource.c_str(), allResourceDataContainer, fileList, progressDialog.get());
		}
	}
#endif //_WIN32
	if (!this->_isResourceExtracted) { //Handle zip extraction error
		wxLogDebug("[Chain-Reaction] Error: resource extraction failed ...");
		ResourceUtilities::clearZipStorage(allResourceDataContainer);
		wxMessageBox(wxT("Failed to extract zip resource. Aborting application ..."), wxT("Resource Handling Error"), wxOK | wxICON_ERROR);
		progressDialog->Destroy();
		return false;
	}
	wxLogDebug("[Chain-Reaction] Resources were extracted successfully ...");
	ResourceUtilities::FileData* audioData = nullptr; //reference to the audio data in resource
	std::unordered_map<std::string, std::string> shaderData; //An unordered map that points each shader file name to its raw string data.
	std::unordered_map<std::string, objl::Material> materialList; //storage location of extracted materials from '.mtl' data.
	unsigned int currentValue = progressDialog->GetValue();
	progressDialog->Update(currentValue + 1, "Loading resources ...");
	std::vector<ResourceUtilities::FileData*> listOfObjectFile;
	//***** Process raw binary resource data and pass them to the wxGLCanvas instance ...
	if (allResourceDataContainer.size() != fileList.size()) {
		std::string errorMsg = allResourceDataContainer.size() > fileList.size() ? "more" : "less";
		wxLogDebug(wxString::Format("[Chain-Reaction] Error: zip resource contains %s than expected number of files. Aborting application ...", errorMsg));
		wxMessageBox(wxString::Format("Zip resource contains %s than expected number of files.", errorMsg), "File entry mismatch!", wxOK | wxICON_ERROR, progressDialog.get());
		ResourceUtilities::clearZipStorage(allResourceDataContainer); //Free all raw data that were extracted from the zip resource.
		progressDialog->Destroy();
		return false;
	}
	for (unsigned int i = 0; i < allResourceDataContainer.size(); i++) {
		ResourceUtilities::FileData* entryData = allResourceDataContainer[i];
		if (!entryData) continue;
		std::string fileName(entryData->name);
		std::string fileExtension = fileName.size() > 4 ? fileName.substr(fileName.size() - 4) : "";
		if (!fileList.count(fileName)) { // Handle error as name of the file is not in the expected list of file names.
			wxMessageBox(wxString::Format("Unsupported file entry '%s' in zip resource file.", fileName.c_str()), "Resource Loading Error!", wxOK | wxICON_ERROR, progressDialog.get());
			ResourceUtilities::clearZipStorage(allResourceDataContainer); //Free all raw data that were extracted from the zip resource.
			progressDialog->Destroy();
			return false;
		}
		else if (!fileExtension.compare(".obj")) { //The data comes from a 3D object file
			listOfObjectFile.push_back(entryData);
		}
		else if (!fileExtension.compare(".wav")) { //the data comes from an audio file
			audioData = entryData; //set the pointer of audioData to the .wav resource.
		}
		else if (!fileExtension.compare(".mtl")) {
			objl::Loader::LoadMaterialsFromMemory((unsigned char*)entryData->data, entryData->size, materialList);
		}
		else if (!fileExtension.compare(".txt")) {//text files are shaders
			shaderData[fileName] = std::string(entryData->data, entryData->data + entryData->size);
		}
		else {
			wxMessageBox(wxString::Format("Unsupported file entry '%s' in resource zip file.", fileName.c_str()), "Resource Loading Error!", wxOK | wxICON_ERROR, progressDialog.get());
			ResourceUtilities::clearZipStorage(allResourceDataContainer);
			progressDialog->Destroy();
			return false;
		}
		currentValue = progressDialog->GetValue();
		progressDialog->Update(currentValue + 1, wxString::Format("Loading resources ... (loaded: %d) ", i + 1));
	}
	//********************************************************************************************
	this->_resourceData.reset(new ResourceUtilities::ResourceData()); //resource data containing text object created from MeshData and default material.
	this->_resourceData->listOfObjectFiles.assign(listOfObjectFile.begin(), listOfObjectFile.end());
	this->_resourceData->listOfAllAvailableMaterials = materialList;
	this->_resourceData->particleShaderVertexSource = shaderData["vertex-shader-particles.txt"].c_str();
	this->_resourceData->particleShaderFragmentSource = shaderData["fragment-shader-particles.txt"].c_str();
	this->_resourceData->textShaderVertexSource = shaderData["vertex-shader-texts.txt"].c_str();
	this->_resourceData->textShaderFragmentSource = shaderData["fragment-shader-texts.txt"].c_str();
	if (audioData == nullptr) { //If audio data was not found while extracting resources, handle error.
		wxLogDebug("[Chain-Reaction] Error: could not locate audio ('.wav') data while extracting resources. Aborting application ...");
		wxMessageBox("Could not locate audio ('.wav') data while extracting resources.", "Resource Loading Error!", wxOK | wxICON_ERROR, progressDialog.get());
		//********** Delete previously allocated memory ****************
		ResourceUtilities::clearZipStorage(allResourceDataContainer); //Free all raw data that were extracted from the zip resource.
		progressDialog->Destroy();
		//*************************************************************
		return false;
	}
	else if (materialList.empty()) { //If material data was not extracted from resources, handle error.
		wxLogDebug("[Chain-Reaction] Error: Could not parse material ('.mtl') data from memory resources. Aborting application ...");
		wxMessageBox("Could not parse material ('.mtl') data from memory resources", "Resource Loading Error!", wxOK | wxICON_ERROR, progressDialog.get());
		//********** Delete previously allocated memory ****************
		ResourceUtilities::clearZipStorage(allResourceDataContainer); //Free all raw data that were extracted from the zip resource.
		progressDialog->Destroy();
		//*************************************************************
		return false;
	}
	else {
		this->_resourceData->defaultSphereMaterial.name = "Plastic"; // use the Plastic material defined in material resource (alphabet.mtl) as a default for the brownian particles.
		for (const auto& material : materialList) {
			if (!(material.first.compare(this->_resourceData->defaultSphereMaterial.name))) {
				this->_resourceData->defaultSphereMaterial = material.second; //stores the extracted Plastic material data to the AppFrame as a resource.
				break;
			}
		}
		wxLogDebug("[Chain-Reaction] Passing the extracted resources to wxGLCanvas for further processing ...");
		double pixelScalingFactor = 1.0;
		if (this->_mainFrame->canvas_panel) {
			this->_mainFrame->canvas_panel->SetResourceData(this->_resourceData.get()); //Pass the object containing resource data (text, material, etc.) to the OpenGL canvas.
			pixelScalingFactor = this->_mainFrame->canvas_panel->GetContentScaleFactor();
		}
		wxLogDebug("[Chain-Reaction] Creating game object ...");
		currentValue = progressDialog->GetValue();
		progressDialog->Update(currentValue + 1, "Creating game object ... ");
		this->_offlineGameObject.reset(new MainGame(audioData->data, audioData->size, shaderData["vertex-shader-gridlines.txt"].c_str(), shaderData["fragment-shader-gridlines.txt"].c_str(), shaderData["vertex-shader-orbs.txt"].c_str(), shaderData["fragment-shader-orbs.txt"].c_str())); //Create offline game object.		
		this->_offlineGameObject->setFrame(this->_mainFrame); //Inform game object about the current GUI frame to maintain back and forth communication.
		this->_offlineGameObject->setPixelScaling(pixelScalingFactor); //Inform game object about the pixel scaling factor of the current OpenGL canvas.
		this->_gameInformation.reset(new GameUtilities::GameState()); //Create object containing runtime game information
		this->_gameInformation->mouse_data_processed = 1;
		this->_gameInformation->game = this->_offlineGameObject.get(); //Attach offline game object
		this->_gameInformation->online_game = nullptr; //Attach online game object
		if (this->_mainFrame->canvas_panel) {
			//Pass the object containing game info to the OpenGL canvas.
			wxLogDebug(wxString::Format("[Chain-Reaction] Passing game information to the wxGLCanvas object %p ...", this->_mainFrame->canvas_panel));
			this->_mainFrame->canvas_panel->SetClientData(this->_gameInformation.get());
		}
		currentValue = progressDialog->GetValue();
		wxLogDebug("[Chain-Reaction] All resources were processed successfully! Clearing memory allocated to the resource storage ...");
		ResourceUtilities::clearZipStorage(allResourceDataContainer); //Free all raw data that were extracted from the zip resource.
		wxLogDebug("[Chain-Reaction] Initializing video system ...");
		progressDialog->Update(currentValue + 1, "All resources were loaded successfully! Initialising video system ...");
		currentValue = progressDialog->GetValue();
		progressDialog->Update(currentValue + 1); //Final update which automatically destroys the progress dialog.
		if (this->_icon.IsOk()) this->_mainFrame->SetIcon(this->_icon);
		return true;
	}
}
bool wxAppInstance::OnInit() {
	//******* Initialize the main GUI frame instance.
	wxLogDebug("[Chain-Reaction] Creating wxWidgets GUI application frame ...");
	this->_mainFrame = new AppGUIFrame("Chain Reaction", wxDefaultPosition, wxDefaultSize, wxSize(630, 600));
	if (this->_mainFrame) {
		this->SetTopWindow(this->_mainFrame);
#ifndef _WIN32 //Include resource header only in Windows environment.
		this->_icon = wxIcon(chainReactionICON);
#endif // !_WIN32
		wxLogDebug(wxString::Format("[Chain-Reaction] GUI application frame %p was successfully created ...", this->_mainFrame));
		wxLogDebug("[Chain-Reaction] Initializing default audio system ...");
		bool isAudioSystemInitialized = SoundSystem::initSystem(); //Initialize default audio system.
		if (!isAudioSystemInitialized) { //Handle error if initialization fails.
			wxMessageBox("Failed to initialize default audio system.", "Audio system failure!", wxOK | wxICON_ERROR);
			return false;
		}
		wxLogDebug("[Chain-Reaction] Audio system was successfully initialized ...");
		wxLogDebug(wxString::Format("[Chain-Reaction] -- Using audio device '%s' ...", SoundSystem::getDeviceName()));
		this->_mainFrame->Iconize(false);
		bool isMainFrameShown = this->_mainFrame->Show(true);
		//Initialize OpenGL context on the canvas.
		bool isOpenGLContextInitialized = false;
		if (this->_mainFrame->canvas_panel) isOpenGLContextInitialized = this->_mainFrame->canvas_panel->InitOpenGLContext();
		if (!isOpenGLContextInitialized) return false;
		this->_isResourceProcessed = this->loadZipResource();
		wxLogDebug(wxString::Format("[Chain-Reaction] Dynamically attaching IDLE event handler to the main application object %p ...", this));
		Bind(wxEVT_IDLE, &wxAppInstance::OnIdle, this, wxID_ANY);
		return isMainFrameShown;
	}
	else {
		wxLogDebug("[Chain-Reaction] Error: failed to create GUI main frame (top window) of the application. Aborting application ...");
		return false;
	}
}
void wxAppInstance::OnIdle(wxIdleEvent &event){
	if (this->_isResourceProcessed) return;
	else{
		wxLogDebug("[Chain-Reaction] Error: resource zip file was either not specified or not successfully extracted. Closing application ...");
		if (this->_mainFrame)
			this->_mainFrame->Close();
		else this->GetTopWindow()->Close();
	}
}
int wxAppInstance::OnExit() {
	wxLogDebug("[Chain-Reaction] Shutting down audio system ...");
	bool isAudioSystemClosed = SoundSystem::shutDown(); //Free memory allocated to create audio objects.
	if (isAudioSystemClosed)
		wxLogDebug("[Chain-Reaction] Audio system was shut down successfully ...");
	else{
		wxLogDebug("[Chain-Reaction] Some error occurred while shutting down audio system ...");
	}
	return wxApp::OnExit();
}
//Enrty point of wxWidget application.
IMPLEMENT_APP(wxAppInstance);
