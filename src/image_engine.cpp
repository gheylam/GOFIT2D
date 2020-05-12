/* 
   Date: 12.05.2020 
   This is the image engine which deals with managing 
   textures. 

*/

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

class Image{
public:
  Image(SDL_Window* window, SDL_Renderer* renderer); //Image init function
  //overloaded constructor which allso takes a string to
  //load the image at the constructor body.
  Image(SDL_Window* window, SDL_Renderer* renderer, std::string path);
  ~Image(void); //Image destructor function

  //inits the SDL library. 
  bool begin(void); 

  //Loads the image from a given string path
  bool loadFile(std::string path);

  //Deallocate image
  void free(void);

  //renders' image at a given point
  void render(int x, int y, SDL_Rect* clip = NULL);

  //Getting methods
  int getWidth();
  int getHeight();

private:
  SDL_Texture* image_texture;
  int image_width;
  int image_height;
  SDL_Window* image_window;
  SDL_Renderer* image_renderer;
};

Image :: Image(SDL_Window* window, SDL_Renderer* renderer){
  //constructor function
  begin(); 
  image_texture = NULL;
  image_width = 0;
  image_height = 0;
  image_window = window;
  image_renderer = renderer; 
}

Image :: Image(SDL_Window* window, SDL_Renderer* renderer, std::string path){
  //overloaded constructor function that also takes the image string path
  begin();
  image_window = window;
  image_renderer = renderer;
  loadFile(path.c_str());		       
}

Image :: ~Image(){
  //Destructor function 
  free();

}

bool Image :: begin(){
  bool success = true;

  if(SDL_Init(SDL_INIT_VIDEO) < 0){
    printf("SDL could not init! SDL Error: %s\n", SDL_GetError()); 
    success = false;
  }else{
    int image_flags = IMG_INIT_PNG | IMG_INIT_JPG;
    if( !IMG_Init(imgage_flags) & image_flags){
      printf("SDL Image could not init! SDL Image Error: %s\n", IMG_GetError());
      success = false;
    }
  }
  return success;
}

//function loads the texture from a given directory path
bool loadFile(std::string path){
  printF("Loading image from path: %s\n", path.c_str());

  //get rid of any pre existing textures;
  free();

  //Instantiate a new texture which we will point to at the end of the function
  SDL_Texture* new_texture = NULL;

  //First load the image as a surface
  SDL_Surface new_surface = IMG_Load(path.c_str());
  if(new_surface == NULL){
    printf("Image could not be loaded at: %s\n, SDL Error: %s\n",
	   path.c_str(), SDL_GetErro());
  }else{
    //convert surface to texture
    new_texture = SDL_CreateTextureFromSurface(image_renderer, new_surface);
    if(new_texture == NULL){
      printf("Could not convert surface to texture: %s\n", SDL_GetError());
    }else{
      texture_width = new_surface->w;
      texture_height = new_surface->h;
    }
    SDL_FreeSurface(new_surface);
  }
  image_texture = new_texture;
  return new_texture != NULL;
}

bool Image :: free(){
  if(image_texture != NULL){
    SDL_DestroyTexture(image_texture);
    image_texture = NULL;
    image_width = 0;
    image_height = 0;
  }
}

int Image :: getWidth(){
  return image_width;
}

int Image :: getHeight(){
  return image_height;
}

void Image :: render(int x, int y, SDL_Rect* clip){
  //Set rendering space and render to screen
  SDL_Rect renderQuad = {x, y, image_width, image_height);

  //set clip rendering dimensions
  if(clip != NULL){
    renderQuad.w = clip->w;
    renderQuad.h = clip->h;
  }

  //render to screen
  SDL_RenderCopy(image_renderer, image_texture, clip, &renderQuad);
}


