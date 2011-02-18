#ifndef CACTORFACTORY_H
#define CACTORFACTORY_H

class CActorFactory
{
public:
  CActor *Create(int actor)
  {
    if (actor == SQUARE)
      return new Square;
    else if (shape == CIRCLE)
      return new Circle;
    else if (shape == TRIANGLE)
      return new Triangle;
  }
};
