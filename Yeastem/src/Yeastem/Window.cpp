#include "Window.h"

wxBEGIN_EVENT_TABLE(YEASTEM::Window, wxFrame)
	EVT_BUTTON(wxID_ANY, YEASTEM::Window::OnButtonClicked)
wxEND_EVENT_TABLE();


namespace YEASTEM
{
	void Window::createImage(wxString imageLocation, int x = 0, int y = 0, double scaleWidth = 1, double scaleHeight = 1) {
		Image img;
		img.image = wxBitmap(imageLocation, wxBITMAP_TYPE_ANY);
		if(scaleWidth != 1 || scaleHeight != 1)
		{
			int width = img.image.GetWidth();
			int height = img.image.GetHeight();
			wxImage Image = img.image.ConvertToImage();

			Image.Rescale(int(width * scaleWidth), int(height * scaleHeight));
			img.image = wxBitmap(Image);
		}
		img.x = x; img.y = y;

		m_images[m_ImagesNum] = img;
		m_ImagesNum ++;
	}

	Window::Window(int posX, int posY, unsigned int width, unsigned int height)
		: wxFrame(nullptr, wxID_ANY, "YEASTEM on TOP!", wxPoint(posX, posY), wxSize(width, height))
	{
		m_panel = new wxPanel(this, wxID_ANY, wxPoint(0, 0), wxSize(width, height));
		m_panel->Bind(wxEVT_PAINT, &Window::OnPaint, this);

		createImage("result.png", width/2, 0, 0.5, 0.5);

		m_button1 = new wxButton(m_panel, wxID_ANY, "Click Me!", wxPoint(10, 10), wxSize(150, 50));
		m_txt1 = new wxTextCtrl(m_panel, wxID_ANY, "I am Text! Wahoo! Yippee!", wxPoint(10, 70), wxSize(300, 30));
		m_list1 = new wxListBox(m_panel, wxID_ANY, wxPoint(10, 110), wxSize(300, 300));
	}

	void Window::OnButtonClicked(wxCommandEvent &evt)
	{
		m_list1->AppendString(m_txt1->GetValue());
		evt.Skip();
	}

	void Window::OnPaint(wxPaintEvent& event) 
	{
		wxPaintDC dc(m_panel);
		dc.Clear();

		for (int i = 0; i < m_ImagesNum; i++) {
			dc.DrawBitmap(m_images[i].image, m_images[i].x, m_images[i].y);
		}
	}

	Window::~Window() {}
}