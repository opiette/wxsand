#include "DownloadFileDialog.h"
#include <wx/protocol/http.h>
#include <wx/txtstrm.h>
#include <wx/textfile.h>
#include <wx/url.h>
#include <wx/fs_inet.h>
#include <wx/filesys.h>

BEGIN_EVENT_TABLE(DownloadFileDialog, wxFrame)
  EVT_BUTTON(1000, DownloadFileDialog::OnOK)
END_EVENT_TABLE()


DownloadFileDialog::DownloadFileDialog(const wxString& title, const wxPoint& pos, const wxSize& size, 
				       long style) : wxFrame(NULL, -1, title, pos, size, style){
  lbPhysicsChoices = new wxListBox(this, 3000, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE);

  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  {
    sizer->Add(lbPhysicsChoices, 1, wxEXPAND);
    sizer->Add(new wxStaticText(this, -1, _("")), 0, wxEXPAND);
    sizer->Add(new wxStaticText(this, -1, _("See details at http://www.fallingsandgame.com/wiki/index.php/WxSand_Mods")), 0, wxEXPAND);
    sizer->Add(new wxStaticText(this, -1, _("")), 0, wxEXPAND);
    sizer->Add(new wxButton(this, 1000, _("OK")), 0, wxCENTER | wxTOP, 2);
  }
  this->SetSizer(sizer);

  wxURL listURL(_("http://www.fallingsandgame.com/wiki/index.php/WxSand_Mods"));
  
  wxProtocol& proto = listURL.GetProtocol();
  wxIPV4address add;
  proto.GetLocal(add);
	//add.Hostname(_("75.126.58.241"));
  //add.Service(80);
  //listURL.SetProxy("207.32.43.252:8080");

  wxInputStream* is = listURL.GetInputStream();
  if (is == NULL){
  	wxMessageDialog* msg;
  
  	msg  = new wxMessageDialog(this, _("Error connecting to the internet:") + add.Hostname() + wxString::Format(_("%d"), listURL.GetError())  );
   	
    msg->ShowModal();
    return;
  }

	


  wxTextInputStream* tis = new wxTextInputStream(*(is));

  wxString line;
  int index = 0;
  while(!line.Contains(_("</body>"))){
    line = tis->ReadLine();
    wxString url = line;
    wxString description1 = line;
    wxString description2 = line;
    if (line.Contains(_("<li>"))){
      int startIndex = line.Find(_("href=\""));
      url = line.Mid(startIndex+6);
      int endIndex = url.Find(_("\""));
      url = url.Mid(0,endIndex);
      urlArray[index] = url;
      ++index;

      startIndex = description1.Find(_("<li>"));
      description1 = description1.Mid(startIndex+4);
      startIndex = description1.Find(_(">"));
      description1 = description1.Mid(startIndex+1);

      endIndex = description1.Find(_("</a>"));
      description1 = description1.Mid(0,endIndex);

      startIndex = description2.Find(_("</a>"));
      description2 = description2.Mid(startIndex+4);

      lbPhysicsChoices->Append(description1 + description2);

    }
  }
  

  //sizer->Fit(this);

  this->physFilename = _("default.txt");
}


void DownloadFileDialog::OnOK(wxCommandEvent& event){
  wxString filename = urlArray[lbPhysicsChoices->GetSelection()];

  wxURL fileURL(_("http://www.fallingsandgame.com") + filename);
  //fileURL.SetProxy("207.32.43.252:8080");

  wxInputStream* is = fileURL.GetInputStream();
  if (is == NULL){
    wxString str = _("Error loading from the file from the internet. ");
    str += _("http://www.fallingsandgame.com") + filename;
    wxMessageDialog* msg  = new wxMessageDialog(this, str);
    msg->ShowModal();
    return;
  }

  wxTextInputStream* tis = new wxTextInputStream(*(is));

  wxTextFile of(_("downloadedPhysics.txt"));
  if (of.Exists()){
    of.Open();
    while(of.GetLineCount() > 0)
    	of.RemoveLine(0);
  }
  else{
    of.Create();
  }

  int state = 0;
  int count = 0;
  while(!is->Eof() && state < 2 && count < 10000){
    wxString line = tis->ReadLine();
    if (line.Contains(_("<pre>"))){
	state = 1;
	continue;
    }
    if (line.Contains(_("</pre>")))
	state = 2;

    if (state == 1){
    	line.Replace(_("&amp;"), _("&"));
    	line.Replace(_("&quot;"), _("\""));
      of.AddLine(line);
    }

    ++count;
  }

  if (state != 2){
    wxMessageDialog* msg  = new wxMessageDialog(this, _("Couldn't find the physics file on the Wiki page."));
    msg->ShowModal();
    of.Write();
    of.Close();
    this->Destroy();
    return;
  }

  of.Write();
  of.Close();


	
  this->physFilename = _("downloadedPhysics.txt");
  physicsFilename = this->physFilename;
  g_mainFrame->loadPhysics(this->physFilename);

  this->Destroy();
}
