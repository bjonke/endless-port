struct SDrawFace 
{
  int current;
  SDrawFace() {current=0;}
	void operator() (RECT i) 
	{
		SelectObject(Tiles_DC,ImageMap["data/images/face_sprites.bmp"].gs_hbmp);
		Blitter.tqueue(BackBuffer, i.left, i.top, 64, 64, Tiles_DC, 96*current, 0, RGB(255,0,255), 0);

		DrawEdge(BackBuffer,&i,BDR_RAISEDINNER,BF_RECT);
		++current;
	}
} SDrawFaceOBj;