#include "CalculateThread.h"	
	
#include <math.h>
#include "Canvas.h"
	
	
	


inline void CalculateThread::drawData(int center){
	particle p = particles[center];
	wxColor color = colors[p.data][p.energy];
  bitmapdata[((center)*3)+0] = color.Red();
  bitmapdata[((center)*3)+1] = color.Green();
  bitmapdata[((center)*3)+2] = color.Blue();
}


inline void CalculateThread::found(particle *p, particle *otherp, int center, int other){
	particle tempp = *p;
	*p = *otherp;
	*otherp = tempp;

  drawData(center);
  drawData(other);
  p->calc = true;
  otherp->calc = true;
}


void CalculateThread::drawCircle(int centerx, int centery){
  int r = pen_width/2;
  int rr = r*r;
  for(int dx=-r;dx<=r;++dx){
    int x = centerx+dx;
    int dy = (int)round(sqrt(rr - dx*dx));
    for(int y=centery-dy;y<=centery+dy;++y){
      if (x >= 0 && x <= g_width-1 && y >= 0 && y <= g_height-1){
				int center = (g_width*y)+x;
				particle *p = &particles[center];
				if (drawEmptyCheckBox->IsChecked() && p->data > 0)
					continue;
				p->data = sand_type;
				p->energy = 0;
				p->xspeed = 0;
				p->yspeed = 0;
				p->calc = false;
				drawData(center);
      }
    }
  }
}

void CalculateThread::drawCircle(int centerx, int centery, int diameter, int sandtype){
  int r = diameter/2;
  int rr = r*r;
  for(int dx=-r;dx<=r;++dx){
    int x = centerx+dx;
    int dy = (int)round(sqrt(rr - dx*dx));
    for(int y=centery-dy;y<=centery+dy;++y){
      if (x >= 0 && x <= g_width-1 && y >= 0 && y <= g_height-1){
				int center = (g_width*y)+x;
				particle *p = &particles[center];
				if (drawEmptyCheckBox->IsChecked() && p->data > 0)
					continue;
				p->data = sandtype;
				p->energy = 0;
				p->xspeed = 0;
				p->yspeed = 0;
				p->calc = false;
				drawData(center);
      }
    }
  }
}









void* CalculateThread::Entry(){

	while(true){

	while(g_isPainting);

  if (mouseIsDown){
    drawCircle(mousex, mousey);
  }

	++frameCount;
 
 	wxPaintEvent pe;
  g_canvas->AddPendingEvent(pe);

  if (doPause == true)
    continue;
   

  if (doSources){
    int space = g_width/(numberOfSources+1);
    for(int i=0;i<10;++i){
      if (rand() < RAND_MAX/4){
				for(int n=0;n<numberOfSources;++n){
					int index = g_width+(space*(n+1))+i;
					particle *p = &particles[index];
					p->data = (unsigned char)sources[n];
					p->energy = 0;
					p->xspeed = 0;
					p->yspeed = 0;
				}
      }
    }
  }
  
  
	int center = g_width - 1 - 1;
	for(int y = 1; y < g_height-1; ++y){
		center += 2;
		for(int x = 1; x < g_width-1; ++x){
		++center;
		
		particle *centerparticle_p = &particles[center];

		if (centerparticle_p->data == 0)
			continue;
		if (centerparticle_p->calc)
			continue;

		//Check for an item.
		
		if (items[centerparticle_p->data].data != 0){
			//An item. Replace it!
			item it = items[centerparticle_p->data];
			//printf("%d\n\n\n", centerparticle_p->data);
			int sx = x - (int)(it.width/2);
			int sy = y - (int)(it.height/2);		
			int i=0;
			
			for(int iy = sy; iy < sy+it.height; ++iy){
				for(int ix = sx; ix < sx+it.width; ++ix){
					//printf("%d ", it.data[i]);
					if (it.data[i] == MAXNUMBEROFELEMENTS-1){
						++i;
						continue;
					}
					int loc = iy*g_width + ix;
					particles[loc].data = it.data[i];
					particles[loc].calc = 1;
					particles[loc].energy = 0;
					particles[loc].xspeed = 0;
					particles[loc].yspeed = 0;
					drawData(loc);
					++i;
				}
				//printf("\n");
			}
		}



		//Check for death.
		if (rand() < cdeath_prob[centerparticle_p->data][centerparticle_p->energy]){
			int r = (int)round(99.0*(double(rand())/double(RAND_MAX)));
			if (death_energy[centerparticle_p->data] != -1)
				centerparticle_p->energy -= death_energy[centerparticle_p->data];
			
			centerparticle_p->data = death_center[centerparticle_p->data][r];
			if (centerparticle_p->data == 0)
				centerparticle_p->energy = 0;
			drawData(center);
			continue;
		}
		
		
		int left = center-1;
		int right = center+1;
		int up = center - g_width;
		int down = center + g_width;
		int downleft = center + g_width - 1;
		int downright = center + g_width + 1;
		int upleft = center - g_width - 1;
		int upright = center - g_width + 1;

		particle* leftparticle = &particles[left];
		particle* rightparticle = &particles[right];
		particle* upparticle = &particles[up];
		particle* downparticle = &particles[down];
		particle* downleftparticle = &particles[downleft];
		particle* downrightparticle = &particles[downright];
		particle* upleftparticle = &particles[upleft];
		particle* uprightparticle = &particles[upright];


		//Do momentum.
		//Momentum just slowly wears out.

		//TODO: Should x change at all??
      
		if (centerparticle_p->xspeed != 0){
			if (centerparticle_p->xspeed > 0){
				centerparticle_p->xspeed -= 1;
				if ( rand() < (double(abs(centerparticle_p->xspeed))/100.0)*RAND_MAX ){
					//if (particles[right].data != 0)
						//if (rand() > RAND_MAX/10){
							//xspeed[center] -= 4;
							//continue;
						//}
		
					found(centerparticle_p, rightparticle, center, right);
					continue;
				}
			}
			else{
				centerparticle_p->xspeed += 1;
				if ( rand() < (double(abs(centerparticle_p->xspeed))/100.0)*RAND_MAX ){
					//if (particles[left].data != 0)
						//if (rand() > RAND_MAX/10){
							//xspeed[center] += 4;
							//continue;
						//}
				
					found(centerparticle_p, leftparticle, center, left);
					continue;
				}
			}
		}
      
		if (centerparticle_p->yspeed != 0){
			if (centerparticle_p->yspeed > 0){
				centerparticle_p->yspeed -= 1;
				if ( rand() < (double(abs(centerparticle_p->yspeed))/100.0)*RAND_MAX ){
					//if (particles[down].data != 0)
						//if (rand() > RAND_MAX/10){
							//yspeed[center] -= 4;
							//continue;
						//}
		
					found(centerparticle_p, downparticle, center, down);
					continue;
				}
			}
			else{
				centerparticle_p->yspeed += 1;
				if ( rand() < (double(abs(centerparticle_p->yspeed))/100.0)*RAND_MAX ){
					//if (particles[up].data != 0)
						//if (rand() > RAND_MAX/10){
							//yspeed[center] += 4;
							//continue;
						//}
		
					found(centerparticle_p, upparticle, center, up);
					continue;
				}
			}
		}
      
     

      
		//Do energy buildups/environment buildup.
		if (doEnergy){
			if (conductivity[centerparticle_p->data] > 1.0){
				int e = centerparticle_p->energy + (int)round(99.0*(conductivity[centerparticle_p->data]-1.0));
				if (e > 99)
					e = 99;
				centerparticle_p->energy = (char)e;
				drawData(center);
			}
			if (rand() < conductivity[centerparticle_p->data]*0.1*RAND_MAX){
				if (leftparticle->data == 0 ||
						rightparticle->data == 0 ||
						upparticle->data == 0 ||
						downparticle->data == 0){
					if (centerparticle_p->energy < g_T)
						centerparticle_p->energy += 1;
					else if (centerparticle_p->energy > g_T)
						centerparticle_p->energy -= 1;
				}
			}
		}


		if (doInteractions){
			//Check for specific transformations.
			//Randomly check neighbors.
			int other = 0;
			int cx = x;
			int cy = y;
			int ox = 0;
			int oy = 0;
				
			unsigned char t1 = centerparticle_p->data;	
				
			for(int i=3;i>=0;--i){
									
				int r = (int)(7.99*double(rand())/RAND_MAX);
				switch(r){
				case(0):
					other = up;
					ox = x;
					oy = y-1;
					break;
				case(1):
					other = down;
					ox = x;
					oy = y+1;
					break;
				case(2):
					other = left;
					ox = x-1;
					oy = y;
					break;
				case(3):
					other = right;
					ox = x+1;
					oy = y;
					break;
				case(4):
					other = upleft;
					ox = x-1;
					oy = y-1;
					break;
				case(5):
					other = downleft;
					ox = x-1;
					oy = y+1;
					break;
				case(6):
					other = downright;
					ox = x+1;
					oy = y+1;
					break;
				case(7):
					other = upright;
					ox = x+1;
					oy = y-1;
					break;
				}
				
				
				unsigned char t2 = particles[other].data;
				
				particle *otherparticle_p = &particles[other];
				
				if (doEnergy){
					if (t2 != 0){
						double c = conductivity[t1];
						if (c > 0){
							if (otherparticle_p->energy > centerparticle_p->energy){
								int de = otherparticle_p->energy-centerparticle_p->energy;
								if (c > 1)
									c = 1;
								de = (int)ceil(c*double(de));
										
								otherparticle_p->energy -= de;
								centerparticle_p->energy += de;
								
								if (centerparticle_p->energy > 99)
									centerparticle_p->energy = 99;
								
								drawData(center);
								drawData(other);
								otherparticle_p->calc = true;
								centerparticle_p->calc = true;
							}
						}
					}
				}
			
				if (pexplosion[t1][t2] > -1){
					drawCircle(cx, cy, pexplosion[t1][t2], pexplosionelement[t1][t2]);
					i=-1;
					continue;
				}
				
				if (rand() < ctrans_prob[t1][t2][centerparticle_p->energy+otherparticle_p->energy]){
					int r = (int)round(99.0*(double(rand())/double(RAND_MAX)));
					if (trans_energy[t1][t2] != -1){
						if (centerparticle_p->energy >= trans_energy[t1][t2])
							centerparticle_p->energy -= trans_energy[t1][t2];
						else
							centerparticle_p->energy = 0;
						
						if (otherparticle_p->energy >= trans_energy[t1][t2])
							otherparticle_p->energy -= trans_energy[t1][t2];
						else
							otherparticle_p->energy = 0;
					}
			
					centerparticle_p->data = trans_center[t1][t2][r];
					otherparticle_p->data = trans_neighbor[t1][t2][r];
					
					if (trans_xspeed[t1][t2] != 0){
						centerparticle_p->xspeed = (int)round(trans_xspeed[t1][t2]*double(rand())/RAND_MAX);
						if (rand() < RAND_MAX/2)
							centerparticle_p->xspeed = -centerparticle_p->xspeed;
			
						otherparticle_p->xspeed = (int)round(trans_xspeed[t1][t2]*double(rand())/RAND_MAX);
						if (rand() < RAND_MAX/2)
							otherparticle_p->xspeed = -otherparticle_p->xspeed;
					}
					
					if (trans_yspeed[t1][t2] != 0){
						centerparticle_p->yspeed = (int)round(trans_yspeed[t1][t2]*double(rand())/RAND_MAX);
						if (rand() < RAND_MAX/2)
							centerparticle_p->yspeed = -centerparticle_p->yspeed;
						otherparticle_p->yspeed = (int)round(trans_yspeed[t1][t2]*double(rand())/RAND_MAX);
						if (rand() < RAND_MAX/2)
							otherparticle_p->yspeed = -otherparticle_p->yspeed;
					}
			
			
					if (centerparticle_p->data == 0){
						centerparticle_p->energy = 0;
						centerparticle_p->xspeed = 0;
						centerparticle_p->yspeed = 0;
					}
					if (otherparticle_p->data == 0){
						otherparticle_p->energy = 0;
						otherparticle_p->xspeed = 0;
						otherparticle_p->yspeed = 0;
					}
					
					drawData(center);
					drawData(other);
					centerparticle_p->calc = true;
					otherparticle_p->calc = true;
					i=-1;
					continue;
				}
			}
				
    } //end doInteractions 
    
    
    
		if (doGravity){
				
			if (gravity[centerparticle_p->data] == 0)
				continue;
		
			//if (centerparticle_p->yspeed < 0)
			//	continue;
	
			//gravity > 0
		
			//falling down
			//density down
			//falling downleft
			//density downleft
			//falling downright
			//density downright
			
			//gravity < 0
		
			//up
			//upleft
			//upright
		
		
			//left open
			//left pushing
			//left density
		
			//right open
			//right pushing
			//right density
	
	
			if (gravity[centerparticle_p->data] > 0){
				//Check down
				//falling
				
					if (downparticle->data == 0){
						if (rand() < cgravity[centerparticle_p->data]){
							found(centerparticle_p, downparticle, center, down);
							continue;
						}
					}
					else{
						//density
						if (density[centerparticle_p->data] > density[downparticle->data]){
							if (rand() < ccogravitydensity[centerparticle_p->data][downparticle->data] ){
								found(centerparticle_p, downparticle, center, down);
								continue;
							}
						}
					}
				
	
				if (rand() < 0.5*RAND_MAX){ //left versus right.
						//Check down and left
						//falling
						if (downleftparticle->data == 0){
							if (rand() < cgravity[centerparticle_p->data]){
								found(centerparticle_p, downleftparticle, center, downleft);
								continue;
							}
						}
						else{
							//density
							if (density[centerparticle_p->data] > density[downleftparticle->data]){
								if ( rand() < ccogravitydensity[centerparticle_p->data][downleftparticle->data] ){
									found(centerparticle_p, downleftparticle, center, downleft);
									continue;
								}
							}
						}
					
				}
				else{//left versus right.
						//Check down and right
						//falling
						if (downrightparticle->data == 0){
							if (rand() < cgravity[centerparticle_p->data]){
								found(centerparticle_p, downrightparticle, center, downright);
								continue;
							}
						}
						else{
							//density
							if (density[centerparticle_p->data] > density[downrightparticle->data]){
								if ( rand() < ccogravitydensity[centerparticle_p->data][downrightparticle->data] ){
									found(centerparticle_p, downrightparticle, center, downright);
									continue;
								}
							}
						}
					
				}
			}
			else{
				//gravity < 0
	
				//up or to the side?
				if (rand() < 0.7*RAND_MAX){
						//Check up
						if (upparticle->data == 0){
							if (rand() < -cgravity[centerparticle_p->data]){
								found(centerparticle_p, upparticle, center, up);
								continue;
							}
						}
					
				}
				else{
					//Check up and left.
					if (rand() < 0.5*RAND_MAX){ //left versus right.
							//floating
							if (upleftparticle->data == 0){
								if (rand() < -cgravity[centerparticle_p->data]){
									found(centerparticle_p, upleftparticle, center, upleft);
									continue;
								}
							}
						
					}
					else{	   //left versus right.
							//Check up and right.
							if (uprightparticle->data == 0){
								if (rand() < -cgravity[centerparticle_p->data]){
									found(centerparticle_p, uprightparticle, center, upright);
									continue;
								}
							}
						
					}
				}
			}
	
			
			if (rand() < 0.5*RAND_MAX){ //left versus right.
				//Check left.
					//Random opening.
					if (leftparticle->data == 0){
						if (rand() < cslip[centerparticle_p->data]){
							found(centerparticle_p, leftparticle, center, left);
							continue;
						}
					}
				
					//density
					if (density[centerparticle_p->data] > density[leftparticle->data]){
						if ( rand() < ccodensity[centerparticle_p->data][leftparticle->data] ){
							found(centerparticle_p, leftparticle, center, left);
							continue;
						}
					}
				
			}
			else{
				//Check right.
					//Random opening.
					if (rightparticle->data == 0){
						if (rand() < cslip[centerparticle_p->data]){
							found(centerparticle_p, rightparticle, center, right);
							continue;
						}
					}
					
					//density
					if (density[centerparticle_p->data] > density[rightparticle->data]){
						if ( rand() < ccodensity[centerparticle_p->data][rightparticle->data] ){
							found(centerparticle_p, rightparticle, center, right);
							continue;
						}
					}
				
			}
		} //doGravity


    
  } //Big loop
}

	//Clean up the edges.
	int clearelement = 0;
	if (doWalls)
		clearelement = MAXNUMBEROFELEMENTS-1;
	
  int bottom = (g_height-1)*g_width;
  for(int x=0;x<g_width;++x){
  	int bot = bottom + x;
  
  	particles[x].data = clearelement;
  	particles[bot].data = clearelement;
  	particles[x].energy = 0;
  	particles[bot].energy = 0;
  	
    drawData(x);
    drawData(bot);
  }

  for(int y=0;y<g_height-1;++y){
    int leftside = y*g_width;
    int rightside = (y*g_width)+g_width-1;

		particles[leftside].data = clearelement;
    particles[rightside].data = clearelement;
    particles[leftside].energy = 0;
    particles[rightside].energy = 0;
    
    drawData(leftside);
    drawData(rightside);
  }
  

  //memset(calc, false, g_width*g_height*sizeof(unsigned char));
	for(int i = (g_width*g_height)-1; i >= 0 ; --i)
  	particles[i].calc = 0;
  
  
  }
  
  
  
  
  return (void*)1;
}

