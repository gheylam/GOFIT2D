/* 
   Date: 08.05.2020 
   This is the first PoC for GOFTT2D. 
   Our aim is to create a simple SDL2 program which will allow us to show a sequence of 
   images with different audio for each image, as well as an overlaying timer. 
   
   Most of the code here will be reused from LazyFoo's tutorials 
*/

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <sstream>

//Function prototypes
bool loadContent(void);
bool init(void);
void closeApp(void);

//Texture Wrapper class
class MyTexture{
public:
  MyTexture(); //texture init function
  ~MyTexture(); //Deallocated texture memory
  
  //loads images at specified path
  bool loadFromFile(std::string path);

  //Deallocate texture
  void free();

  //renders texture at given point
  void render(int x, int y, SDL_Rect* clip = NULL);

  //Creates image from font string
  bool loadFromRenderedText(std::string texture_text, SDL_Color text_color); 
  
  //image dimension exposure API
  int getWidth();
  int getHeight();

private:
  SDL_Texture* texture;
  int texture_width;
  int texture_height;
};

//global variables
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 720;
const int total_exercises = 3; 

MyTexture exercise_textures[total_exercises];
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

//The music that will be played
Mix_Music *audio_demo_1 = NULL;
Mix_Music *audio_demo_2 = NULL;
Mix_Music *audio_demo_3 = NULL;

//globally used font
TTF_Font * sdl_font = NULL;

//Rendered text texture
MyTexture text_texture; 

//Timing feature variables

MyTexture time_texture;

//Current time start time
Uint32 start_time = 0;

//In memory text stream
std :: stringstream time_text;


MyTexture :: MyTexture(){
  //constructor function
  texture = NULL;
  texture_width = 0;
  texture_height = 0;
}

MyTexture :: ~MyTexture(){
  //destructor function
  free();
}

bool MyTexture :: loadFromFile(std::string path){
  printf("loading image from path: %s\n", path.c_str());

  //get rid of any preexisting textures;
  free();
  SDL_Texture* newTexture = NULL;

  //Load the image as a surface
  SDL_Surface* loadedSurface = IMG_Load(path.c_str());
  if(loadedSurface == NULL){
    printf("Visual could not be loaded at: %s\n, SDL Error: %s\n", path.c_str(),
	  SDL_GetError());
  }else{
    //Convert the surface into a texture
    newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if(newTexture == NULL){
      printf("Could not convert surface into texture: %s\n", SDL_GetError());
    }else{
      texture_width = loadedSurface->w;
      texture_height = loadedSurface->h;
    }
    SDL_FreeSurface(loadedSurface);
  }
  //return the newly converted texture
  texture = newTexture;
  return texture != NULL;
}

void MyTexture :: free(){
  //Frees the texture
  if(texture != NULL){
    SDL_DestroyTexture(texture);
    texture = NULL;
    texture_width = 0;
    texture_height = 0;
  }
}

int MyTexture :: getWidth(){
  return texture_width;
}

int MyTexture :: getHeight(){
  return texture_height;
}

void MyTexture :: render(int x, int y, SDL_Rect* clip){
  //Set rendering space and render to screen
  SDL_Rect renderQuad = {x, y, texture_width, texture_height};

  //set clip rendering dimensions
  if( clip != NULL ){
    renderQuad.w = clip->w;
    renderQuad.h = clip->h;
  }

  //render to screen
  SDL_RenderCopy(renderer, texture, clip, &renderQuad);
}

bool loadMedia(){
  bool success = true;
  for(int i = 0; i < total_exercises; i++){
    std::string image_file_path = "./visuals/circuit_demo_" + std::to_string(i+1) + ".jpg";
    success = exercise_textures[i].loadFromFile(image_file_path);
  }
  //load music
  audio_demo_1 = Mix_LoadMUS("./audio/audio_demo_1.mp3");
  if(audio_demo_1 == NULL){
    printf("failed to load audio demo 1 SDL Mixer Error: %s\n", Mix_GetError());
    success = false;
  }
  audio_demo_2 = Mix_LoadMUS("./audio/audio_demo_2.mp3");
  if(audio_demo_2 == NULL){
    printf("failed to load audio demo 2 SDL Mixer Error: %s\n", Mix_GetError());
    success = false;
  }
  audio_demo_3 = Mix_LoadMUS("./audio/audio_demo_3.mp3");
  if(audio_demo_3 == NULL){
    printf("failed to load audio demo 3, SDL Mixer Error: %s\n", Mix_GetError());
    success = false;
  }

  //Open the font
  sdl_font = TTF_OpenFont("font/LemonMilk.ttf", 24);
  if(sdl_font == NULL){
    printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
    success = false; 
  }
  return success;
}

bool init(void){
  bool success = true;

  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0){
    printf("SDL could not init! SDL Error: %s\n", SDL_GetError());
    success = false;
  }else{
    //create window
    window = SDL_CreateWindow("MOE-FIT", SDL_WINDOWPOS_UNDEFINED,
			      SDL_WINDOWPOS_UNDEFINED,
			      SCREEN_WIDTH,
			      SCREEN_HEIGHT,
			      SDL_WINDOW_SHOWN);
    if(window == NULL){
      printf("Window could not be init! SDL Error: %s\n", SDL_GetError());
      success = false;
    }else{
      //create renderer for rendering context
      renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
      if(renderer == NULL){
	printf("Renderer could not init! SDL Error: %s\n", SDL_GetError());
	success = false;
      }else{
	//init IMG library
	int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
	if( !IMG_Init(imgFlags) & imgFlags){
	  printf("SDL Image could not init, SDL_image error: %s\n", IMG_GetError());
	  success = false;
	}
	//Init Mixer libary
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
	  printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
	  success = false;
	}
	//Init TTF
	if(TTF_Init() == -1) {
	  printf("SDL_TTF could not init! SDL_TTF error: %s\n", TTF_GetError());
	  success = false;
	}
	
      }
    }
  }
  return success;
}

void close(void){
  //Destory our textures
  for(int i = 0; i < total_exercises; i++){
    exercise_textures[i].free();
  }
  //Free the music
  Mix_FreeMusic(audio_demo_1);
  Mix_FreeMusic(audio_demo_2);
  Mix_FreeMusic(audio_demo_3);
  audio_demo_1 = NULL;
  audio_demo_2 = NULL;
  audio_demo_3 = NULL;

  //Free the text texture
  text_texture.free();
  TTF_CloseFont(sdl_font);
  sdl_font = NULL;
  
  //Destroy window and renderer
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);

  window = NULL;
  renderer = NULL;

  //quit SDL subsystems
  TTF_Quit();
  Mix_Quit();
  IMG_Quit();
  SDL_Quit();
}

int main(int argc, char* args[]){
  //holder for whether the game quits
  bool quit = false;
  int exercise_number = 0;
  bool first_frame = true;
  SDL_Color text_color = {0,0,0,255};
  //holder for SDL events
  SDL_Event e;

  if(!init()){
    printf("init function failed\n");
    quit = true;
  }else{
    if(!loadMedia()){
      printf("loading media failed\n");
      quit = true;
    }
  }
  //starting the game
  while(!quit){
    //handle the events on the event queue
    
    while(SDL_PollEvent(&e) != 0){
      if(e.type == SDL_QUIT){
	quit = true;
      }
      //user presses a key
      if(e.type == SDL_KEYDOWN){
	if(e.key.keysym.sym == SDLK_RETURN){
	  exercise_number++;
	  switch(exercise_number){
	  case 1:
	    Mix_PlayMusic(audio_demo_2, 1);
	    break;
	  case 2:
	    Mix_PlayMusic(audio_demo_3, 1);
	    break;
	  default:
	    break;
	  }
	  printf("Exercise number: %d\n", exercise_number+1);
	  start_time = SDL_GetTicks();
	}
      }
    }
    /*look through the keystates
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
    if(currentKeyStates[SDL_SCANCODE_RETURN]){
      exercise_number++;
      printf("exercise number: %d\n", exercise_number);
    
    }
    */
    if(exercise_number < 3){
      //input code to render the context here;
      SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
      SDL_RenderClear(renderer);
      
      exercise_textures[exercise_number].render(0,0, NULL);

      

      //update screen
      
      time_text.str("");
      time_text << "Time Elapsed: " << (SDL_GetTicks() - start_time)/1000;
      if(!time_texture.loadFromRenderedText(time_text.str().c_str(), text_color)){
	printf("Unable to render time texture\n");
      }
      time_texture.render((SCREEN_WIDTH - text_texture.getWidth())/2,
			  (SCREEN_HEIGHT - text_texture.getHeight())/2);
      
      
      SDL_RenderPresent(renderer);
      if(first_frame){
	Mix_PlayMusic(audio_demo_1, 1);
	first_frame = false;
	start_time = SDL_GetTicks();
      }
    }else{
      quit = true;
    }
  }
  close();
}
  
//Loads a texture from a text string
bool MyTexture::loadFromRenderedText(std::string texture_text, SDL_Color text_color){
   //Get rid of preexisting textures
   free();

   //Render text surface
   SDL_Surface* text_surface = TTF_RenderText_Solid(sdl_font, texture_text.c_str(), text_color);
   if(text_surface == NULL){
     printf("Unable to render surface from string, SDL Error: %s\n", TTF_GetError());
   }else{
     //Create texture from surface pixels
     texture = SDL_CreateTextureFromSurface(renderer, text_surface);
     if(texture == NULL){
       printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
     }else{
       //Get image dimensions
       texture_width = text_surface->w;
       texture_height = text_surface->h;
     }

     //Get rid of old surface
     SDL_FreeSurface(text_surface);
   }
   //Return success
   return texture != NULL;
     
 }
