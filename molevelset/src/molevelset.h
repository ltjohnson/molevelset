#ifndef MOLEVELSET_H
#define MOLEVELSET_H

double inset_cost(box *p, double gamma, double delta, double rho, double A);
double inset_risk(box *p, double gamma, double A);
double complexity_penalty(box *p, double gamma, double delta);

#endif
