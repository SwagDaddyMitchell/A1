// state.cpp

#include "headers.h"

#include "state.h"

// Draw each of the objects in the world

void State::draw()

{
  int i;

  gpuProgram->activate();

  glUniformMatrix4fv(glGetUniformLocation(gpuProgram->id(), "M"), 1, GL_TRUE, &M[0][0]);

  silos[0].draw(gpuProgram);

  for (i = 0; i < silos.size(); i++)
    silos[i].draw(gpuProgram);

  for (i = 0; i < cities.size(); i++)
    cities[i].draw(gpuProgram);

  for (i = 0; i < missilesIn.size(); i++)
    missilesIn[i].draw(gpuProgram);

  for (i = 0; i < missilesOut.size(); i++)
    missilesOut[i].draw(gpuProgram);

  for (i = 0; i < explosions.size(); i++)
    explosions[i].draw(gpuProgram);

  gpuProgram->deactivate();
}

// Update the state of the world after some time interval, deltaT
//
// CHANGE ALL OF THIS FUNCTION

void State::updateState(float deltaT)

{
  int i;

  // Update the time

  currentTime += deltaT;

  // Generate some new missiles.  The rate of missile generation
  // should increase with time.
  //
  // CHANGE THIS

  if (randIn01() > 0.99)
  { // New missile

    float randXsrc = randIn01();
    std::cout << "SOURCE X: " << randXsrc << std::endl;

    //the following calculates slopes of lines from (random X source, worldTop) to (0, 0) and (0, 1)
    //This so for any randomly generated x component of missile vector a Y component can be generated
    // such that the resulting vector will terminate within the screen dimensions on the line y = 0

    float yGenBound1 = worldTop / (randXsrc); //slope of line from src to 0,0
    float yGenBound2 = -worldTop / (1 - randXsrc); // slope of line from src to 0,1

    float randXvec = randIn01(); 
    float randYvec = 0;

    if (randXvec < randXsrc)
    { 
      // if x component of vector is left of source X coord
      randYvec = yGenBound1 * randXvec * randIn01();
    }
    else if (randXvec > randXsrc)
    {
      // if x component of vector is right of source X coord
      randYvec = (yGenBound2 * (randXvec - randXsrc) + worldTop) * randIn01();
    }
    else
    {
      //if it is the same as the source x coord
      randYvec = worldTop * randIn01();
    }
    std::cout << "x: " << randXvec;
    std::cout << " y: " << randYvec << std::endl;
    

    missilesIn.add(Missile(vec3(randXsrc, worldTop, 0),                                         // source
                           0.4 * vec3(randXvec - randXsrc, randYvec - worldTop, 0).normalize(), // velocity
                           0,                                                                   // destination y
                           vec3(1, 1, 0)));                                                     // colour
  }

  // Look for terminating missiles

  for (i = 0; i < missilesIn.size(); i++)
    if (missilesIn[i].hasReachedDestination())
    {
      // CHANGE THIS: ADD AN EXPLOSION

      //explosions.add(new Circle(vec3 p, float s, float maxRad, vec3 c));
      missilesIn.remove(i);
      i--;
    }

  for (i = 0; i < missilesOut.size(); i++)
    if (missilesOut[i].hasReachedDestination())
    {
      // CHANGE THIS: ADD AN EXPLOSION
      missilesOut.remove(i);
      i--;
    }

  // Look for terminating explosions

  for (i = 0; i < explosions.size(); i++)
    if (explosions[i].radius() >= explosions[i].maxRadius())
    {
      // CHANGE THIS: CHECK FOR DESTROYED CITY OR SILO
      explosions.remove(i);
      i--;
    }

  // Look for incoming missiles that hit an explosion and are
  // destroyed

  // ADD CODE HERE

  // Update all the moving objects

  for (i = 0; i < missilesIn.size(); i++)
    missilesIn[i].move(deltaT);

  for (i = 0; i < missilesOut.size(); i++)
    missilesOut[i].move(deltaT);

  for (i = 0; i < explosions.size(); i++)
    explosions[i].expand(deltaT);
}

// Fire a missile

void State::fireMissile(int siloIndex, float x, float y)

{
  const float speed = 0.3;

  if (silos[siloIndex].canShoot())
  {

    silos[siloIndex].decrMissiles();

    // CHANGE THIS

    missilesOut.add(Missile(silos[siloIndex].position(),                                                                       // source
                            speed * vec3(x - silos[siloIndex].position().x, y - silos[siloIndex].position().y, 0).normalize(), // velocity
                            y,                                                                                                 // destination y
                            vec3(0, 1, 1)));                                                                                   // colour
  }
}

// Create the initial state of the world

void State::setupWorld()

{
  // Keep track of the time

  currentTime = 0;

  timeOfIncomingFlight = 6; // 6 seconds for incoming missile to arrive

  // Seed the random number generator

#ifdef _WIN32
  srand(12345);
#else
  struct timeb t;
  ftime(&t);
  srand(t.time);
#endif

  // Silos are added in order left, middle, right so that they have
  // indices 0, 1, and 2 (respectively) in the "silos" array.

  silos.add(Silo(vec3(0.1, 0, 0)));
  silos.add(Silo(vec3(0.5, 0, 0)));
  silos.add(Silo(vec3(0.9, 0, 0)));

  // Cities on the left

  cities.add(City(vec3(0.2, 0, 0)));
  cities.add(City(vec3(0.3, 0, 0)));
  cities.add(City(vec3(0.4, 0, 0)));

  // Cities on the right

  cities.add(City(vec3(0.6, 0, 0)));
  cities.add(City(vec3(0.7, 0, 0)));
  cities.add(City(vec3(0.8, 0, 0)));
}
