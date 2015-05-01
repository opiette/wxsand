#include "MainFrame.h"
#include "Sand.h"
#include "Canvas.h"
#include "DownloadFileDialog.h"

#include <math.h>

#include "refresh.xpm"
#include "pause.xpm"
#include <wx/image.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>

#define TIMERINTERVAL 33

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_SET_FOCUS(MainFrame::OnMove)
  EVT_SIZE(MainFrame::OnSize)
  EVT_LISTBOX(1000, MainFrame::OnGroupChoice)
  EVT_LISTBOX(1001, MainFrame::OnPenChoice)

  EVT_SPINCTRL(1010, MainFrame::OnPenSize)
 

  EVT_SPINCTRL(1020, MainFrame::OnTempSpin)

  EVT_BUTTON(1025, MainFrame::OnButton)
  EVT_BUTTON(1026, MainFrame::OnToggleButton)

  EVT_MENU(1002, MainFrame::OnMenu)
  EVT_MENU(1003, MainFrame::OnMenu)
  EVT_MENU(1004, MainFrame::OnMenu)
  EVT_MENU(1005, MainFrame::OnMenu)
  EVT_MENU(1006, MainFrame::OnMenu)
  EVT_MENU(1007, MainFrame::OnMenu)
  EVT_MENU(1008, MainFrame::OnMenu)

  EVT_MENU(1050, MainFrame::OnMenu)
  EVT_MENU(1051, MainFrame::OnMenu)
  EVT_MENU(1052, MainFrame::OnMenu)
  EVT_MENU(1053, MainFrame::OnMenu)
  EVT_MENU(1054, MainFrame::OnMenu)
  EVT_MENU(1055, MainFrame::OnMenu)
  EVT_MENU(1056, MainFrame::OnMenu)
  EVT_MENU(1057, MainFrame::OnMenu)

  EVT_CLOSE(MainFrame::OnClose)
END_EVENT_TABLE()


MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size, 
		     long style) : wxFrame(NULL, -1, title, pos, size, style){
  g_mainFrame = this;

  wxMenu* fileMenu = new wxMenu;
  wxMenu* optionsMenu = new wxMenu;
  wxMenu* helpMenu = new wxMenu;
  
  fileMenu->Append(1050, _("New.."), _(""));
  fileMenu->AppendSeparator();
  fileMenu->Append(1051, _("Load sandbox.."), _(""));
  fileMenu->Append(1052, _("Save sandbox.."), _(""));
  fileMenu->AppendSeparator();
  fileMenu->Append(1055, _("Load physics.."), _(""));
  fileMenu->Append(1057, _("Load physics from online wiki.."), _(""));
  //fileMenu->Append(1056, _("Save physics.."), _(""));
  fileMenu->AppendSeparator();
  fileMenu->Append(1053, _("Exit"), _(""));

  wallsCB = new wxMenuItem(optionsMenu, 1002, _("Boundary Walls"), _(""), wxITEM_CHECK);
  sourcesCB = new wxMenuItem(optionsMenu, 1003, _("Sources"), _(""), wxITEM_CHECK);
  drawCB = new wxMenuItem(optionsMenu, 1004, _("Paint Updates"), _(""), wxITEM_CHECK);
  gravityCB = new wxMenuItem(optionsMenu, 1005, _("Gravity"), _(""), wxITEM_CHECK);
  energyCB = new wxMenuItem(optionsMenu, 1008, _("Energy"), _(""), wxITEM_CHECK);
  interactionsCB = new wxMenuItem(optionsMenu, 1006, _("Element Interactions"), _(""), wxITEM_CHECK);
  limitCB = new wxMenuItem(optionsMenu, 1007, _("Limit FPS"), _(""), wxITEM_CHECK);

  optionsMenu->Append(wallsCB);
  optionsMenu->Append(sourcesCB);
  optionsMenu->Append(drawCB);
  optionsMenu->Append(gravityCB);
  optionsMenu->Append(energyCB);
  optionsMenu->Append(interactionsCB);
  optionsMenu->Append(limitCB);

  helpMenu->Append(1054, _("About"), _(""));

  wxMenuBar* menuBar = new wxMenuBar();
  menuBar->Append(fileMenu, _("File"));
  menuBar->Append(optionsMenu, _("Options"));
  menuBar->Append(helpMenu, _("Help"));

  SetMenuBar(menuBar);

  wallsCB->Check();
  sourcesCB->Check();
  drawCB->Check();
  gravityCB->Check();
  energyCB->Check();
  interactionsCB->Check();
  limitCB->Check();

  doWalls = wallsCB->IsChecked();
  doSources = sourcesCB->IsChecked();
  doDraw = drawCB->IsChecked();
  doGravity = gravityCB->IsChecked();
  doEnergy = energyCB->IsChecked();
  doInteractions = interactionsCB->IsChecked();
  doLimit = limitCB->IsChecked();

  statusbar = this->CreateStatusBar();
  statusbar->SetFieldsCount(3);
  statusbar->Show(true);

  g_canvas = new Canvas(this, -1, wxDefaultPosition, wxSize(g_width, g_height));
  
  wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
  {
    wxBoxSizer* RHSizer = new wxBoxSizer(wxVERTICAL);
    {
      
      wxBoxSizer* RH1 = new wxBoxSizer(wxHORIZONTAL);
      {
				RH1->Add(new wxBitmapButton(this, 1025, wxBitmap(refresh_xpm)), 0, wxCENTER | wxALL, 10);
				RH1->Add(new wxBitmapButton(this, 1026, wxBitmap(pause_xpm)), 0, wxCENTER | wxALL, 10);
      }

      wxStaticBox* sb4 = new wxStaticBox(this, -1, _("Sandbox Temperature"));
      wxStaticBoxSizer* RH5 = new wxStaticBoxSizer(sb4, wxHORIZONTAL);
      {
				wxSpinCtrl* ctrl = new wxSpinCtrl(this, 1020);
				RH5->Add(ctrl);
				ctrl->SetValue(22);
      }

      wxStaticBox* sb1 = new wxStaticBox(this, -1, _("Pen Options"));
      wxStaticBoxSizer* RH2 = new wxStaticBoxSizer(sb1, wxHORIZONTAL);
      {
      	wxBoxSizer* RH21 = new wxBoxSizer(wxHORIZONTAL);
				{
					RH21->Add(new wxStaticText(this, -1, _("Pen Size: ")), 0, wxCENTER | wxLEFT, 2);
					wxSpinCtrl* ctrl = new wxSpinCtrl(this, 1010);
					RH21->Add(ctrl);
					ctrl->SetValue(4);
					//RH2->Add(new wxRadioButton(this, 1019, _("512")), 0, wxCENTER | wxLEFT, 2);
				}
				RH2->Add(RH21, 1, wxCENTER | wxLEFT, 2);
					
				drawEmptyCheckBox = new wxCheckBox(this, -1, _("Draw only in empty"));
				RH2->Add(drawEmptyCheckBox, 1, wxCENTER | wxLEFT, 2);
				drawEmptyCheckBox->SetValue(false);
			}
      
      
      wxStaticBox* sb2 = new wxStaticBox(this, -1, _("Element Selection"));
      wxStaticBoxSizer* RH3 = new wxStaticBoxSizer(sb2, wxHORIZONTAL);
      {
				groupSelections = new wxListBox(this, 1000, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE);
				penSelections = new wxListBox(this, 1001, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE);
			
				RH3->Add(groupSelections, 1, wxEXPAND);
				RH3->Add(penSelections, 1, wxEXPAND);
      }
      
      wxStaticBox* sb3 = new wxStaticBox(this, -1, _("Element Description"));
      wxStaticBoxSizer* RH4 = new wxStaticBoxSizer(sb3, wxHORIZONTAL);

      elementDescription = new wxTextCtrl(this, -1, _("Erases.                                                       "), 
					  wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE);
      RH4->Add(elementDescription, 1, wxEXPAND);

      RHSizer->Add(RH1, 0);
      RHSizer->Add(RH5, 0, wxALL | wxEXPAND, 5);
      RHSizer->Add(RH2, 0, wxALL, 5);
      RHSizer->Add(RH3, 1, wxEXPAND);
      RHSizer->Add(RH4, 0, wxEXPAND | wxALL, 5);
    }

    sizer->Add(g_canvas, 0, wxADJUST_MINSIZE | wxALL, 10);
    sizer->Add(RHSizer, 1, wxEXPAND);
  }
  this->SetSizer(sizer);
  
  this->Layout();
  sizer->Fit(this);

  g_timer = new wxTimer(g_canvas, 999);
  g_timer->Start(TIMERINTERVAL);

  g_timer_second = new wxTimer(g_canvas, 998);
  g_timer_second->Start(1000);

}

void MainFrame::OnTempSpin(wxSpinEvent& event){
  g_T = event.GetPosition();
}

void ShowPenSelections(){
  penSelections->Clear();

  int firstVisible = -1;
  for(int i=0;i<numberOfGroupItems[group_type];++i){
    int n_element = groups[group_type][i];
    if (visible[n_element]){
      if (firstVisible == -1)
				firstVisible = n_element;
      penSelections->Append(names[n_element]);
    }
  }
  penSelections->SetSelection(0);
  sand_type = firstVisible;
  elementDescription->SetValue(elementDescriptions[sand_type]);

  //todo:Get this to work.
  g_mainFrame->Layout();
}



void RefreshPenList(){
  groupSelections->Clear();
  penSelections->Clear();
  
  bool visibleGroups[MAXNUMBEROFELEMENTS];
  for(int i=0;i<numberOfGroups;++i){
    visibleGroups[i] = false;
  }

	int firstVisibleGroup=-1;
  for(int k=0;k<numberOfGroups;++k){
    for(int i=0;i<numberOfGroupItems[k];++i){
    	int n_element = groups[k][i];
      if (visible[n_element]){
      	if (firstVisibleGroup == -1)
	  			firstVisibleGroup = k;
				visibleGroups[k] = true;
      }
    }
  }

  for(int i=0;i<numberOfGroups;++i)
    if (visibleGroups[i])
      groupSelections->Append(groupNames[i]);
  
  group_type = firstVisibleGroup;
  groupSelections->SetSelection(0);

  //Now show pens.
  ShowPenSelections();

  group_type = 0;
  sand_type = 0;
  
  //todo: fix this
  g_mainFrame->Layout();
}


void MainFrame::loadSandbox(wxString filename){
  wxImage loadBitmap;
  loadBitmap.LoadFile(filename);
  g_width = loadBitmap.GetWidth();
  g_height = loadBitmap.GetHeight();
  drawAll = true;

  for(int y=0;y<g_height;++y){
    for(int x=0;x<g_width;++x){
      int i = (y*g_width)+x;
      
      particles[i].energy = 0;
      particles[i].data = 0;

      //Find the closest.
      unsigned int smallestDistance = 256+256+256;
      
      unsigned char red = loadBitmap.GetRed(x,y);
      unsigned char green = loadBitmap.GetGreen(x,y);
      unsigned char blue = loadBitmap.GetBlue(x,y);
      
      if (red == 0 && green == 0 && blue == 0)
				continue;
      
      for(int j=0;j<numberOfElements;++j){
				unsigned int dist = 
					int(abs(red - colors[j][0].Red())) + 
					int(abs(green - colors[j][0].Green())) + 
					int(abs(blue - colors[j][0].Blue()));
				if (dist < smallestDistance){
					smallestDistance = dist;
					particles[i].data = j;
				}
      }
    }
  }
  
  g_canvas->SetSizeHints(g_width,g_height,g_width,g_height);
  g_canvas->Initialize();
  //g_canvas->SetClientSize(g_width,g_height);
  g_mainFrame->Layout();
  g_mainFrame->GetSizer()->Fit(g_mainFrame);

}


void loadFileError(int lineNumber, const wxString message){
  wxString str(_(""));
  str.Printf(_("Error on line %d: %s"), lineNumber, message.c_str());

  wxMessageDialog dlg(g_mainFrame, str, _("Error"), wxOK);
  dlg.ShowModal();

  for(int i=0;i<MAXNUMBEROFELEMENTS;++i){
    numberOfGroupItems[i] = 0;
    conductivity[i] = 0;
    visible[i] = 0;
    death_energy[i] = -1;
    death_prob[i] = 0;

    for(int j=0;j<MAXNUMBEROFELEMENTS;++j){
      trans_energy[i][j] = -1;
      trans_prob[i][j] = 0;
      trans_xspeed[i][j] = 0;
      trans_yspeed[i][j] = 0;
      pexplosion[i][j] = -1;

      for(int k=0;k<100;++k){
				trans_center[i][j][k] = 0;
				trans_neighbor[i][j][k] = 0;
				death_center[i][k] = 0;
				ctrans_prob[i][j][k] = 0;
				
				cdeath_prob[i][k] = 0;
				colors[i][k] = wxColor(0,0,0);
      }
    }
  }

  numberOfGroups = 0;
  numberOfElements = 0;

	for(int i=0;i<MAXSIZE;++i){
		particles[i].data = 0;
		particles[i].energy = 0;
	}
  
}


void clearPhysics(){
  for(int i=0;i<MAXNUMBEROFELEMENTS;++i){
    sources[i] = 0;
    conductivity[i] = 0;
    visible[i] = 1;
    death_energy[i] = -1;
    death_prob[i] = 0;

		if (items[i].data != 0){
			delete[] items[i].data;
			items[i].data = 0;
		}
		items[i].width = 0;
		items[i].height = 0;

    for(int j=0;j<MAXNUMBEROFELEMENTS;++j){
      trans_energy[i][j] = -1;
      trans_prob[i][j] = 0;
      trans_xspeed[i][j] = 0;
      trans_yspeed[i][j] = 0;
      pexplosion[i][j] = -1;

      for(int k=0;k<100;++k){
				trans_center[i][j][k] = 0;
				trans_neighbor[i][j][k] = 0;
				death_center[i][k] = 0;
				cdeath_prob[i][k] = 0;
				ctrans_prob[i][j][k] = 0;
			
				colors[i][k] = wxColor(0,0,0);
      }
    }
  }
  numberOfGroups = 0;
  numberOfElements = 0;
  numberOfSources = 0;

}


int getElementNumber(wxString name, int lineNumber, wxTextFile &file){
	if (name == _("all") || name == _("All"))
		return -2;

	int n_element = -1;
	for(int i=0;i<numberOfElements;++i){
		if (names[i] == name)
			n_element = i;
	}

	if (n_element == -1){
		loadFileError(lineNumber, _("Unknown element:") + name);
		file.Close();
		return -1;
	}
	
	return n_element;
}



void MainFrame::loadPhysics(wxString filename){
  clearPhysics();

  wxTextFile file;
  if (!file.Open(filename)){
    loadFileError(0, _("Could not open file."));
    return;
  }

  wxString line;
  //wxStringTokenizer linetok;

  //First, find out what version of file this is.
  int version = 0;

  for (line = file.GetFirstLine(); !file.Eof(); line = file.GetNextLine()){
    if (line.GetChar(0) == wxChar('#')){
      if (line.Find(_("subversion 1")) != wxNOT_FOUND)
				version = 1;
      else if (line.Find(_("subversion 2")) != wxNOT_FOUND)
				version = 2;
    }
  }

  if (version == 0){
    loadFileError(0, _("Could not find a line that says \"subversion #\"."));
    file.Close();
    return;
  }

  if (version == 1){


    //TODO


  }
  else if (version == 2){
    //Then, count the number of elements and get their names.
    numberOfGroups = 1;
    groupNames[0] = _("Misc");
    numberOfGroupItems[0] = 0;
    numberOfElements = 0;
    int state = 0; //0:normal, 1:reading first line after an item, 2:reading lines after an item.
		
    wxString comment = _("");

    for (line = file.GetFirstLine(); !file.Eof(); line = file.GetNextLine()){ 
      if (line.GetChar(0) == '#'){
				//comment.
				comment = wxString(line).Mid(1);
				state = 0;
      }
      else if(line == _("") || line.GetChar(0) == ' ' || line.GetChar(0) == '\t' || line.GetChar(0) == '\n' || line.GetChar(0) == '\r'){
				//skip
				comment = _("");
				state = 0;
      }
      else{
				if (state == 1){
      		//type line.
      		//assign colors and characters. Later.
      		state = 2;      		
				}
				else if (state == 2){
					//type line
					//"  .wwssww.  ",
					//calculate width and height.
      		items[numberOfElements-1].width = line.find_last_of('"') - 1;
      		//printf("w=%d\n", items[numberOfElements-1].width);
      		items[numberOfElements-1].height += 1;
      	}
				else{
      	
					wxStringTokenizer linetok(line, _(" \t\n\r"));
					wxString type = linetok.GetNextToken();

					if (type == _("element")){
						if (numberOfElements == MAXNUMBEROFELEMENTS-2){
							loadFileError(0, _("Error: Too many elements and items. Limit of ") + wxString::Format(_("%d"), MAXNUMBEROFELEMENTS-2) + _(". Ignoring \"") + linetok.GetNextToken() + _("\" the rest."));
						}
						else{
							wxString name = linetok.GetNextToken();
							names[numberOfElements] = name;
							elementDescriptions[numberOfElements] = comment;
							
							items[numberOfElements].data = 0;
			  	
							long int red;
							long int green;
							long int blue;
							
							linetok.GetNextToken().ToLong(&red);
							linetok.GetNextToken().ToLong(&green);
							linetok.GetNextToken().ToLong(&blue);
							
							linetok.GetNextToken().ToDouble(&gravity[numberOfElements]);
							linetok.GetNextToken().ToDouble(&slip[numberOfElements]);
							linetok.GetNextToken().ToDouble(&density[numberOfElements]);
							linetok.GetNextToken().ToDouble(&conductivity[numberOfElements]);
							
							if (linetok.GetNextToken() == _("1"))
								visible[numberOfElements] = true;
							else
								visible[numberOfElements] = false;
					
							for(int k=0;k<100;++k){
								colors[numberOfElements][k] = wxColour((unsigned char)red, 
											(unsigned char)green, 
											(unsigned char)blue);
							}
				
							++numberOfElements;
							//printf("%s\n", (char*)name.c_str());
						}
					}
					else if (type == _("group")){
						wxString name = linetok.GetNextToken();
						groupNames[numberOfGroups] = name;
						++numberOfGroups;
						comment = _("");
					}
					else if (type == _("item")){
						if (numberOfElements == MAXNUMBEROFELEMENTS-2){
							loadFileError(0, _("Error: Too many elements and items. Limit of ") + wxString::Format(_("%d"), MAXNUMBEROFELEMENTS-2) + _(". Ignoring \"") + linetok.GetNextToken() + _("\" the rest."));
						}
						else{
							wxString name = linetok.GetNextToken();
							names[numberOfElements] = name;
							elementDescriptions[numberOfElements] = comment;
							++numberOfElements;
							//printf("%s\n", (char*)name.c_str());
							state = 1;
						}
					}
					else{
						comment = _("");
					}
					
				}//else state
      }
    }
    
    numberOfSources = 0;
    
    //Now fill in the elements.
    int elementNumber = 0;
    int lineNumber = 0;
    state = 0;
		char elementCharacters[256];
		int itemDataIndex = 0;
		
    for (line = file.GetFirstLine(); !file.Eof(); line = file.GetNextLine()){ 
      ++lineNumber;
      wxStringTokenizer linetok(line, _(" \t\n\r"));
      
      if (line == _("\r\n") || line == _("\n") || line == _("") || line.GetChar(0) == '#' || line.GetChar(0) == ' ' || line.GetChar(0) == '\t' || line.GetChar(0) == '\n' || line.GetChar(0)== '\r'){
				//skip.
				state = 0;
				continue;
      }
      
      
			if (state == 1){
				//type line.
				//assign colors and characters.
				
				while(linetok.HasMoreTokens()){
					wxString element = linetok.GetNextToken();
					char elementCharacter = linetok.GetNextToken().c_str()[0];
					
					int n_element = getElementNumber(element, lineNumber, file);
					if (n_element == -1) return;
					
					elementCharacters[(int)elementCharacter] = n_element;
					elementCharacters[(int)' '] = MAXNUMBEROFELEMENTS-1;
					//printf("%c , %d = %d\n", elementCharacter, (int)elementCharacter, n_element);
				}
				//TODO:Don't forget to delete this data!!!!!
				//printf("creating item data %d x %d.\n", items[elementNumber-1].width , items[elementNumber-1].height);
				items[elementNumber-1].data = new unsigned char[items[elementNumber-1].width * items[elementNumber-1].height];
				itemDataIndex = 0;
				state = 2;
			}
			else if (state == 2){
				//type line
				//Store data.
				for(unsigned int i=1;i<=items[elementNumber-1].width;++i){
					//printf("%d %d %c %d\n", elementNumber-1, itemDataIndex, line[i], elementCharacters[line[i]]);
					items[elementNumber-1].data[itemDataIndex] = elementCharacters[line[i]];
					itemDataIndex++;
				}
			}
      else{
				wxString type = linetok.GetNextToken();
							
				if (type == _("item")){
					if (elementNumber <= MAXNUMBEROFELEMENTS-2){
					
						linetok.GetNextToken(); //name
						
						if (linetok.GetNextToken() == _("1"))
							visible[elementNumber] = true;
						else
							visible[elementNumber] = false;
							
						state = 1;
						++elementNumber;
					}
				}
				else if (type == _("element")){
					//Already did this above.	
					++elementNumber;
				}
				else if (type == _("hotcolor")){
					long int red;
					long int green;
					long int blue;
					
					wxString center = linetok.GetNextToken();
					
					linetok.GetNextToken().ToLong(&red);
					linetok.GetNextToken().ToLong(&green);
					linetok.GetNextToken().ToLong(&blue);
					
					int n_center = getElementNumber(center, lineNumber, file);
      		if (n_center == -1) return;
      		
      		for(int i_center = 0; i_center < numberOfElements; ++i_center){
      			if (n_center == i_center || (n_center == -2 && i_center > 0)){
							unsigned char ored = colors[i_center][0].Red();
							unsigned char ogreen = colors[i_center][0].Green();
							unsigned char oblue = colors[i_center][0].Blue();
					
							for(int k=1;k<100;++k){
								double nred = ored+(double(k)/99.0)*(red-ored);
								double ngreen = ogreen+(double(k)/99.0)*(green-ogreen);
								double nblue = oblue+(double(k)/99.0)*(blue-oblue);
					
								colors[i_center][k] = wxColour((unsigned char)nred, 
												(unsigned char)ngreen, 
												(unsigned char)nblue);
							}
						}
					}
				}
				else if (type == _("sources")){
					
					while(linetok.HasMoreTokens()){
						wxString element = linetok.GetNextToken();
						
						int n_element = getElementNumber(element, lineNumber, file);
      			if (n_element == -1) return;
			
						sources[numberOfSources] = n_element;
						++numberOfSources;
					}
					
				}
				else if (type == _("group")){
				
					wxString group = linetok.GetNextToken();
					
					if (group  == _("")){
						loadFileError(lineNumber, _("Invalid group line."));
						file.Close();
						return;
					}
			
					int n_group = -1;
					for(int i=0;i<numberOfGroups;++i){
						if (groupNames[i] == group)
							n_group = i;
					}
					
					numberOfGroupItems[n_group] = 0;
			
					while(linetok.HasMoreTokens()){
						wxString element = linetok.GetNextToken();
						
						int n_element = getElementNumber(element, lineNumber, file);
      			if (n_element == -1) return;
			
						groups[n_group][numberOfGroupItems[n_group]] = n_element;
						++numberOfGroupItems[n_group];
					}
					
				}
				else if (type == _("self")){
					//deathrate. "self [prob] [element] [prob][element] ..."
					
					wxString thisprob = linetok.GetNextToken();
					wxString center = linetok.GetNextToken();
					
					if (thisprob  == _("") || center  == _("")){
						loadFileError(lineNumber, _("Invalid self line."));
						file.Close();
						return;
					}
			
					double n_thisprob = 0;
					thisprob.ToDouble(&n_thisprob);
					
					int n_center = getElementNumber(center, lineNumber, file);
      		if (n_center == -1) return;

					for(int i_center = 0; i_center < numberOfElements; ++i_center){
      			if (n_center == i_center || (n_center == -2 && i_center > 0)){
							death_prob[i_center] = n_thisprob;
						}
					}
			
					int currentProbIndex = 0;
					while(linetok.HasMoreTokens()){
						wxString prob = linetok.GetNextToken();
						wxString transelement = linetok.GetNextToken();
						
						if (transelement  == _("")){
							loadFileError(lineNumber, _("Invalid self line. Need at least 1 element."));
							file.Close();
							return;
						}
			
						double n_prob;
						prob.ToDouble(&n_prob);
									
						int n_transelement = getElementNumber(transelement, lineNumber, file);
      			if (n_transelement == -1) return;
			
						for(int i_center = 0; i_center < numberOfElements; ++i_center){
      				if (n_center == i_center || (n_center == -2 && i_center > 0)){
								//Fill the probability array.
								for(int i=currentProbIndex; i < 100 && i < currentProbIndex + int(round(100.0*n_prob)); ++i){
									death_center[i_center][i] = n_transelement;
								}
							}
						}
						currentProbIndex = currentProbIndex + int(round(100.0*n_prob));
					}
						
					if (currentProbIndex != 100){
						loadFileError(lineNumber, _("Invalid line. All probabilities must add up to 1."));
						file.Close();
						return;
					}
				}
				else if (type == _("hotself")){
					//deathrate. "hotself [prob] [element]"
					wxString thisprob = linetok.GetNextToken();
					wxString center = linetok.GetNextToken();
					
					if (thisprob  == _("") || center  == _("")){
						loadFileError(lineNumber, _("Invalid hotself line."));
						file.Close();
						return;
					}
			
					long int n_thisprob = 0;
					thisprob.ToLong(&n_thisprob);
					
					int n_center = getElementNumber(center, lineNumber, file);
      		if (n_center == -1) return;
					for(int i_center = 0; i_center < numberOfElements; ++i_center){
						if (n_center == i_center || (n_center == -2 && i_center > 0)){
							death_energy[n_center] = n_thisprob;
						}
					}
				}
				else if (type == _("neighbor")){
					//neighbor [prob] [element] [element] [prob][element][element] ...
					//Reading element interactions.
					wxString thisprob = linetok.GetNextToken();
					wxString center = linetok.GetNextToken();
					wxString neighbor = linetok.GetNextToken();
						
				
					if (thisprob  == _("") || center  == _("") || neighbor  == _("")){
						loadFileError(lineNumber, _("Invalid neighbor line."));
						file.Close();
						return;
					}
			
					double n_thisprob = 0;
					thisprob.ToDouble(&n_thisprob);
				
					int n_center = getElementNumber(center, lineNumber, file);
      		if (n_center == -1) return;
      		
      		int n_neighbor = getElementNumber(neighbor, lineNumber, file);
      		if (n_neighbor == -1) return;
			
					for(int i_center = 0; i_center < numberOfElements; ++i_center){
						if (n_center == i_center || (n_center == -2 && i_center > 0)){
						
							for(int i_neighbor = 0; i_neighbor < numberOfElements; ++i_neighbor){
								if (n_neighbor == i_neighbor || (n_neighbor == -2 && i_neighbor > 0)){
						
									trans_prob[i_center][i_neighbor] = n_thisprob;
								}
							}
						}
					}
					
			
					int currentProbIndex = 0;
					while(linetok.HasMoreTokens()){
						wxString prob = linetok.GetNextToken();
						wxString s_trans_center = linetok.GetNextToken();
						wxString s_trans_neighbor = linetok.GetNextToken();
			
						double n_prob;
						prob.ToDouble(&n_prob);
				
				
						int n_trans_center = getElementNumber(s_trans_center, lineNumber, file);
						if (n_trans_center == -1) return;
						
						int n_trans_neighbor = getElementNumber(s_trans_neighbor, lineNumber, file);
						if (n_trans_neighbor == -1) return;
			
						for(int i_center = 0; i_center < numberOfElements; ++i_center){
							if (n_center == i_center || (n_center == -2 && i_center > 0)){
							
								for(int i_neighbor = 0; i_neighbor < numberOfElements; ++i_neighbor){
									if (n_neighbor == i_neighbor || (n_neighbor == -2 && i_neighbor > 0)){
								
										//Fill the probability array.
										for(int i=currentProbIndex; i < 100 && i < currentProbIndex + int(round(100.0*n_prob)); ++i){
											trans_center[i_center][i_neighbor][i] = n_trans_center;
											trans_neighbor[i_center][i_neighbor][i] = n_trans_neighbor;
										}
									}
								}
							}
						}
						currentProbIndex = currentProbIndex + int(round(100.0*n_prob));
					
					}
					if (currentProbIndex != 100){
						loadFileError(lineNumber, _("Invalid line. All probabilities must add up to 1."));
						file.Close();
						return;
					}
					
				}
				else if (type == _("hotneighbor")){
					//hotneighbor [prob] [element] [element] 
					//Reading element interactions.
					wxString thisprob = linetok.GetNextToken();
					wxString center = linetok.GetNextToken();
					wxString neighbor = linetok.GetNextToken();
				
					if (thisprob  == _("") || center  == _("") || neighbor  == _("")){
						loadFileError(lineNumber, _("Invalid hotneighbor line."));
						file.Close();
						return;
					}
			
					long int n_thisprob = 0;
					thisprob.ToLong(&n_thisprob);
			
					int n_center = getElementNumber(center, lineNumber, file);
      		if (n_center == -1) return;
      		
      		int n_neighbor = getElementNumber(neighbor, lineNumber, file);
      		if (n_neighbor == -1) return;
					
					for(int i_center = 0; i_center < numberOfElements; ++i_center){
						if (n_center == i_center || (n_center == -2 && i_center > 0)){
						
							for(int i_neighbor = 0; i_neighbor < numberOfElements; ++i_neighbor){
								if (n_neighbor == i_neighbor || (n_neighbor == -2 && i_neighbor > 0)){
									trans_energy[i_center][i_neighbor] = n_thisprob;
								}
							}
						}
					}
				}
				else if (type == _("pexplosion")){
					//pexplosion [element] [element] [diameter] [element]
					//Reading element interactions.
					wxString center = linetok.GetNextToken();
					wxString neighbor = linetok.GetNextToken();
					wxString diameter = linetok.GetNextToken();
					wxString pelement = linetok.GetNextToken();
					if (center  == _("") || neighbor  == _("") || diameter  == _("") || pelement  == _("")){
						loadFileError(lineNumber, _("Invalid pexplosion line."));
						file.Close();
						return;
					}
			
					int n_center = getElementNumber(center, lineNumber, file);
      		if (n_center == -1) return;
      		
      		int n_neighbor = getElementNumber(neighbor, lineNumber, file);
      		if (n_neighbor == -1) return;
      		
      		int n_pelement = getElementNumber(pelement, lineNumber, file);
      		if (n_pelement == -1) return;
					
					long int n_diameter = 0;
					diameter.ToLong(&n_diameter);
			
					for(int i_center = 0; i_center < numberOfElements; ++i_center){
						if (n_center == i_center || (n_center == -2 && i_center > 0)){
						
							for(int i_neighbor = 0; i_neighbor < numberOfElements; ++i_neighbor){
								if (n_neighbor == i_neighbor || (n_neighbor == -2 && i_neighbor > 0)){
									pexplosion[i_center][i_neighbor] = (short int)n_diameter;
									pexplosionelement[i_center][i_neighbor] = (unsigned char)n_pelement;
								}
							}
						}
					}
					
				}
				else if (type == _("explosion")){
					//explosion [element] [element] 
					//Reading element interactions.
					wxString center = linetok.GetNextToken();
					wxString neighbor = linetok.GetNextToken();
					wxString xsp = linetok.GetNextToken();
					wxString ysp = linetok.GetNextToken();
				
					if (center  == _("") || neighbor  == _("") || xsp  == _("") || ysp  == _("")){
						loadFileError(lineNumber, _("Invalid explosion line."));
						file.Close();
						return;
					}
			
					int n_center = getElementNumber(center, lineNumber, file);
      		if (n_center == -1) return;
      		
      		int n_neighbor = getElementNumber(neighbor, lineNumber, file);
      		if (n_neighbor == -1) return;
					
					long int n_xsp = 0;
					xsp.ToLong(&n_xsp);
			
					long int n_ysp = 0;
					ysp.ToLong(&n_ysp);
			
					for(int i_center = 0; i_center < numberOfElements; ++i_center){
						if (n_center == i_center || (n_center == -2 && i_center > 0)){
						
							for(int i_neighbor = 0; i_neighbor < numberOfElements; ++i_neighbor){
								if (n_neighbor == i_neighbor || (n_neighbor == -2 && i_neighbor > 0)){
									trans_xspeed[i_center][i_neighbor] = (unsigned char)n_xsp;
									trans_yspeed[i_center][i_neighbor] = (unsigned char)n_ysp;
								}
							}
						}
					}
					
				}
				else{
					//loadFileError(lineNumber, _("Invalid line. Unrecognized keyword. Ignoring."));
					//file.Close();
					//return;
					wxString str(_(""));
					str.Printf(_("Error on line %d: Invalid line. Unrecognized keyword. Ignoring."), lineNumber);
					wxMessageDialog dlg(g_mainFrame, str, _("Error"), wxOK);
					dlg.ShowModal();
				}

      }
    }
  
  }

  //Figure out the misc group.
  bool grouped[MAXNUMBEROFELEMENTS];
  for(int i=0;i<numberOfElements;++i)
    grouped[i] = false;

  for(int i=0;i<numberOfGroups;++i){
    for(int j=0;j<numberOfGroupItems[i];++j){
      grouped[groups[i][j]] = true;
    }
  }

  for(int i=0;i<numberOfElements;++i){
    if (!grouped[i]){
      groups[0][numberOfGroupItems[0]] = i;
      numberOfGroupItems[0] += 1;
    }
  }

  //Precalculate some commonly used probabilities.
  for(int i=0;i<numberOfElements;++i){
    cgravity[i] = (int)round(gravity[i]*RAND_MAX);
    cslip[i] = (int)round(slip[i]*RAND_MAX);

    for(int k=0;k<100;++k){
      if (death_energy[i] == -1)
				cdeath_prob[i][k] = (int)round(death_prob[i]*RAND_MAX);
      else{
				//double inc = death_prob[i]/(99.0-double(death_energy[i]));
				//printf("%f\n", inc);
				if (k > death_energy[i]){
					cdeath_prob[i][k] = (int)round(death_prob[i]* RAND_MAX); //(int)round(double(k-death_energy[i]) * inc * RAND_MAX);
				}
				else{
					cdeath_prob[i][k] = 0;
				}
				//printf("%d,", cdeath_prob[i][k]);
      }
    }
    //printf("\n");
    //cdeath_prob[i] = (int)round(death_prob[i]*RAND_MAX);

    for(int j=0;j<numberOfElements;++j){
      ccogravitydensity[i][j] = (int)round( fabs(gravity[i])*fabs(density[i]-density[j])*RAND_MAX );
      //probability that these will switch places.
      //High difference = high prob. Low difference=low prob.
      ccodensity[i][j] = (int)round( fabs(density[i]-density[j])*RAND_MAX );

      

      //double inc = trans_prob[i][j]/(99.0-double(trans_energy[i][j]));
      for(int k=0;k<100;++k){
				if (trans_energy[i][j] == -1)
					ctrans_prob[i][j][k] = (int)round(trans_prob[i][j]*RAND_MAX);
				else if (k > trans_energy[i][j]){
					ctrans_prob[i][j][k] = (int)round(trans_prob[i][j]*RAND_MAX); //(int)round(double(k-trans_energy[i][j]) * inc) * RAND_MAX;
				}
				else{
					ctrans_prob[i][j][k] = 0;
				}
      }

      //printf("%s + %s @ %d = \n", (char*)(names[i].c_str()), (char*)(names[j].c_str()), ctrans_prob[i][j][0]);

      //ctrans_prob[i][j] = (int)round(trans_prob[i][j]*RAND_MAX);
    }
  }
  

  /*
  //Print out the interactions.
  for (int i=0;i<numberOfElements;++i){
    for (int j=0;j<numberOfElements;++j){
      char* ni = (char*)(names[i].c_str());
      char* nj = (char*)(names[j].c_str());

      printf("%s + %s @ %f = \n", ni, nj, trans_prob[i][j]);
      for (int k=0;k<100;++k){
	printf("%d ", trans_center[i][j][k]);
      }
      printf("\n");
      for (int k=0;k<100;++k){
	printf("%d ", trans_neighbor[i][j][k]);
      }
      printf("\n");

    }
  }
  */

  
  //Update selections.
  RefreshPenList();
  
  file.Close();



  

}

bool OnNew(wxWindow* window){
  //New
  wxString choices[] = {_("320x240 (fastest)"), _("640x480"), _("800x600"), _("1024x768"), _("1280x1024 (slowest)")};
  wxSingleChoiceDialog dialog(NULL, _("Select an area size"), _("New"), 5, choices, NULL);
  
  if (dialog.ShowModal() == wxID_OK){
    if (dialog.GetSelection() == 0){
      g_width = 320;
      g_height = 240;
    }
    else if (dialog.GetSelection() == 1){
      g_width = 640;
      g_height = 480;
    }
    else if (dialog.GetSelection() == 2){
      g_width = 800;
      g_height = 600;
    }
    else if (dialog.GetSelection() == 3){
      g_width = 1024;
      g_height = 768;
    }
    else if (dialog.GetSelection() == 4){
      g_width = 1280;
      g_height = 1024;
    }
    g_canvas->Initialize();
    return true;
  }
  return false;
}




void MainFrame::OnMenu(wxCommandEvent& event){

  if (event.GetId() == 1050){
    OnNew(this);

    drawAll = true;
    
		for(int i=0;i<MAXSIZE;++i){
			particles[i].data = 0;
			particles[i].energy = 0;
		}

    g_canvas->SetSizeHints(g_width,g_height,g_width,g_height);
    //g_canvas->SetClientSize(g_width,g_height);
    g_mainFrame->Layout();
    g_mainFrame->GetSizer()->Fit(g_mainFrame);
    
    g_canvas->Refresh();
  }
  else if(event.GetId() == 1051){
    //Load
    wxFileDialog dialog(this, _("Load from a file"), _(""), _(""), _("Any Image Files (*.*)|*.*"), wxOPEN);
    
    if (dialog.ShowModal() == wxID_OK){
      sandboxFilename = dialog.GetPath();
      loadSandbox(sandboxFilename);
    }
  }
  else if(event.GetId() == 1052){
    //Save
    wxFileDialog dialog(this, _("Save to a file"), _(""), _(""), _("PNG files (*.png)|*.png|BMP files (*.bmp)|*.bmp"), wxSAVE);
    
    if (dialog.ShowModal() == wxID_OK){
      wxString filename = dialog.GetPath();
      unsigned char* bitmapdata2 = new unsigned char[g_width*g_height*3];
      for(int i=0;i<g_width*g_height;++i){
      	int d = particles[i].data;
				bitmapdata2[(i*3)] = colors[d][0].Red();
				bitmapdata2[(i*3)+1] = colors[d][0].Green();
				bitmapdata2[(i*3)+2] = colors[d][0].Blue();
      }
      wxImage saveBitmap(g_width, g_height, bitmapdata2, true);
      wxBitmap bmp(saveBitmap);

      if (dialog.GetFilterIndex() == 0 || filename.Find(_(".png")) != -1 || filename.Find(_(".PNG")) != -1){
				bmp.SaveFile(filename, wxBITMAP_TYPE_PNG);
      }
      else{
				bmp.SaveFile(filename, wxBITMAP_TYPE_BMP);
      }

      delete bitmapdata2;
    }
  }
  else if(event.GetId() == 1054){
    //About
    wxString str = _("");
    str.Printf(_("Owen Piette's Falling Sand Game\nCopyright Owen Piette 2006.\nVersion %s, compiled on %s\nCheck for updates at:\nhttp://www.piettes.com/fallingsandgame/\nThanks to coppro for his contributions.\nThanks to Troy Larson for the refresh button idea.\nThanks to purple100 for pexplosion.\nThanks to all the people at www.fallingsandgame.com for their support.\n\n"), VERSION, CVERSION);
    wxMessageDialog dlg(this, str, _("About"));
    dlg.ShowModal();
  }
  else if(event.GetId() == 1055){
    //Load physics

    wxFileDialog dialog(this, _("Load from a file"), _(""), _(""), _("Physics Files (*.txt)|*.txt"), wxOPEN);
    if (dialog.ShowModal() == wxID_OK){
      physicsFilename = dialog.GetPath();
      loadPhysics(physicsFilename);
    }
  }
  else if(event.GetId() == 1056){
    //Save physics

    wxMessageDialog dlg(g_mainFrame, _("Sorry, the save feature has been temporarily disabled in this version."), _("Error"), wxOK);
    dlg.ShowModal();

    /*
    wxFileDialog dialog(this, _("Save to a file"), _(""), _(""), _("Physics files (*.txt)|*.txt"), wxSAVE);
    
    if (dialog.ShowModal() == wxID_OK){
      wxString filename = dialog.GetPath();
      FILE* file = fopen(filename, "w");
      
      if (file == NULL){
	wxMessageDialog dlg(g_mainFrame, _("Could not open file to save."), _("Error"), wxOK);
	dlg.ShowModal();
	return;
      }
      fprintf(file, "#wxSand: Owen Piette's Falling Sand Game\n#Version %s-%s, File subversion 2\n", VERSION, CVERSION);
      
      //groups
      for(int i=0;i<numberOfGroups; ++i){
	fprintf(file, "group\t%s", groupNames[i].c_str());
	for(int j=0;j<numberOfGroupItems[i];++j){
	  int n = groups[i][j];
	  fprintf(file, "\t%s", names[n].c_str());
	}
	fprintf(file, "\n");
      }

      //sources
      fprintf(file, "sources");
      for(int i=0;i<numberOfSources; ++i){
	fprintf(file, "\t%s", names[sources[i]].c_str());
      }
      fprintf(file, "\n");

      for(int i=0;i<numberOfElements;++i){
	//element
	fprintf(file, "element\t%s\t%d\t%d\t%d\t%f\t%f\t%f\t%f\t%d\n", 
		names[i].c_str(), colors[i][0].Red(), colors[i][0].Green(), colors[i][0].Blue(), 
		gravity[i], slip[i], density[i], conductivity[i], (int)visible[i]);

	//deathrate. "self [prob] [element] [prob][element] ..."
	if (death_prob[i] > 0){
	  fprintf(file, "self\t%f\t%s", death_prob[i], names[i].c_str());
	  int sum = 1;
	  int prev_index = death_center[i][0];
	  for(int k=1;k<100;++k){
	    if (prev_index == death_center[i][k]){
	      ++sum;
	    }
	    else{
	      fprintf(file, "\t%f\t%s", double(sum)/100.0, names[death_center[i][k-1]].c_str());
	      sum = 1;
	    }
	    prev_index = death_center[i][k];
	  }
	  fprintf(file, "\t%f\t%s", double(sum)/100.0, names[death_center[i][99]].c_str());

	  fprintf(file, "\n");
	}

	//Neighbors
	for(int j=0;j<numberOfElements;++j){
	  //neighbor [prob] [element] [element] [prob][element][element] ...
	  if (trans_prob[i][j] != 0){
	    fprintf(file, "neighbor\t%f\t%s\t%s", trans_prob[i][j], names[i].c_str(), names[j].c_str());
	    int sum = 1;
	    int prev_index = trans_center[i][j][0];
	    for(int k=1;k<100;++k){
	      if (prev_index == trans_center[i][j][k]){
		++sum;
	      }
	      else{
		fprintf(file, "\t%f\t%s\t%s", double(sum)/100.0, names[trans_center[i][j][k-1]].c_str(), 
			names[trans_neighbor[i][j][k-1]].c_str());
		sum = 1;
	      }
	      prev_index = trans_center[i][j][k];
	    }
	    fprintf(file, "\t%f\t%s\t%s", double(sum)/100.0, names[trans_center[i][j][99]].c_str(), 
		    names[trans_neighbor[i][j][99]].c_str());
	    fprintf(file, "\n");
	  }
	  if (trans_energy[i][j] != 0){
	    fprintf(file, "hotneighbor\t%d\t%s\t%s\n", trans_energy[i][j], names[i].c_str(), names[j].c_str());
	  }
	}

	//Hotcolor
	if (colors[i][0] != colors[i][99]){
	  fprintf(file, "hotcolor\t%s\t%d\t%d\t%d\n", names[i].c_str(), colors[i][99].Red(), colors[i][99].Green(), colors[i][99].Blue());
	}

	//Hotself
	if (death_energy[i] != 0){
	  fprintf(file, "hotself\t%d\t%s\n", death_energy[i], names[i].c_str());
	}
      
      }
      fprintf(file, "\n");
      file.Close();
    }
    */      

  }
  else if(event.GetId() == 1057){
    DownloadFileDialog* dp = new DownloadFileDialog(_("Download a physics file"), wxPoint(100,100), wxSize(500,500));
    dp->Show(true);
  }
  else if(event.GetId() == 1053){
    //Exit
    delete g_timer;
    delete g_timer_second;

    g_mainFrame->Close(true);
  }
  
  if (event.GetId() != 1053){
  
		doWalls = wallsCB->IsChecked();
		doSources = sourcesCB->IsChecked();
		doDraw = drawCB->IsChecked();
		doGravity = gravityCB->IsChecked();
		doEnergy = energyCB->IsChecked();
		doInteractions = interactionsCB->IsChecked();
		doLimit = limitCB->IsChecked();
	
		if (doLimit){
			if (!(g_timer->IsRunning()))
				g_timer->Start(TIMERINTERVAL);
		}
		else
			if ((g_timer->IsRunning()))
				g_timer->Stop();
  }
}

void MainFrame::OnButton(wxCommandEvent& event){
  if (event.GetId() == 1025){
    wxMessageDialog dlg(this, _("Are you certain?"), _("Refresh and Reload"), wxYES_NO);
    if (dlg.ShowModal() == wxID_YES){
      if(sandboxFilename == _("")){
				//Refresh
				drawAll = true;
				
				for(int i=0;i<MAXSIZE;++i){
					particles[i].data = 0;
					particles[i].energy = 0;
				}

      }
      else{
				//reload
				loadSandbox(sandboxFilename);
      }
      
      if (physicsFilename != _("")){
				loadPhysics(physicsFilename);
      }
    }
  }
}

void MainFrame::OnToggleButton(wxCommandEvent& event){
  if (event.GetId() == 1026){
    if (doPause == false)
      doPause = true;
    else
      doPause = false;
  }
}

void MainFrame::OnGroupChoice(wxCommandEvent& event){
  wxString groupName = event.GetString();
  group_type = 0;
  for(int i=0;i<numberOfGroups;++i){
    if (groupNames[i] == groupName){
      group_type = i;
      break;
    }
  }

  ShowPenSelections();
}

void MainFrame::OnPenChoice(wxCommandEvent& event){
  wxString penName = event.GetString();
  sand_type = 0;
  for(int i=0;i<numberOfElements;++i){
    if (names[i] == penName){
      sand_type = i;
      break;
    }
  }

  elementDescription->SetValue(elementDescriptions[sand_type]);
}

void MainFrame::OnPenSize(wxSpinEvent& event){
  pen_width = event.GetPosition();
  
}

void MainFrame::OnSize(wxSizeEvent& event){
  doResize = true;
  drawAll = true;
  g_mainFrame->Layout();
}

void MainFrame::OnMove(wxFocusEvent& event){
  drawAll = true;
  if (g_canvas){
    g_canvas->Refresh();
  }
}

void MainFrame::OnClose(wxCloseEvent& event){
  FILE* file = fopen("settings.ini", "w");
  if (!file)
    return;
  fprintf(file, "Boundary Walls:%d\n", (int)doWalls);
  fprintf(file, "Sources:%d\n", (int)doSources);
  fprintf(file, "Paint Updates:%d\n", (int)doDraw);
  fprintf(file, "Gravity:%d\n", (int)doGravity);
  fprintf(file, "Energy:%d\n", (int)doEnergy);
  fprintf(file, "Element Interactions:%d\n", (int)doInteractions);
  fprintf(file, "Limit FPS:%d\n", (int)doLimit);
  fclose(file);

	this->Destroy();
}
