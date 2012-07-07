#include "findtree.h"

#include <vector>
#include <map>

vector<MOBox *> FindTree(double *px, double *py, int n, int dim, int kmax,
			 double gamma, double rho, double delta) {
  /* Initialize the hash for each level of the algorithm. */
  vector<map<string, MOBox *> *> D;
  D.push_back(FindBoxes(n, dim, kmax, px, py));

  /* Collapse one level at a time. */
  for(int i = 0; i < dim * kmax; i++) {
    D.push_back(CollapseLevel(D.at(i), dim, kmax, gamma, rho, delta));
  }

  /* Extract minimax optimal tree. */
  /* Cleanup. */
  for(int i = 0; i < D.size(); i++)
    delte D.at(i);

  return(COMPUTED VECTOR OF BOXES THAT IS THE TREE);
}

MOBox *MakeParentBox(MOBox *pBox, MOBox *pSib, int i, 
		     double gamma, double delta, double rho) {
  if (pSib && pSib->GetBoxKey() != pBox->GetSiblingKey(i)) {
    // THROW EXCEPTION
  }
  MOBox *pParent = pBox->CreateParentBox(i);
  if (pSib) {
    pParent->AddPoint(pSib->GetPoints);
    if (pParent->Cost(gamma, delta, rho).cost > 
	pBox->Cost(gamma, delta, rho).cost + 
	pSib->Cost(gamma, delta, rho).cost) {
      pParent->SetChildren(pBox, pSib);
    }
  }
  return(pParent);
}

map<string, MOBox *> *CollapseLevel(map<string, MOBox *> *pOldLevel, int dim, 
				    int kmax, double gamma, double rho, 
				    double delta) {
  map<string, MOBox *> *pNewLevel = new map<string, MOBox *>;
  map<string, MOBox *>::iterator it, sib, parent;
  for(it=pOldLevel->first(); it != pOldLevel->end(); it++) {
    MOBox *pBox = it->second;
    for(int i = 0; i < dim; i++) {
      /* don't double check splits, and don't check non-existant splits */
      if (pBox->GetChecked(i))
	continue;
      pBox->SetChecked(i);
      MOBox *pSib = NULL;
      string sibKey = pBox->GetSiblingKey(i);
      sib = pOldLevel->find(pBox->GetSiblingKey(i));
      if (sib != pOldLevel->end()) {
	pSib = sib->second();
	pSib->SetChecked(i);
      }
      MOBox *pParent = MakeParentBox(pBox, pSib, i, gamma, delta, rho);
      parent = pNewLevel->find(pParent->GetBoxKey());
      if (parent == pNewLevel->end()) {
	/* Parent not there, add new level. */
	pNewLevel->insert(pair<string, MOBox *>(parentKey, pParent));
      } else {
	/* A parent box was already there, keep the parent box with the
	   lower cost, delete the other. */
	MOBox *pDelete, *pKeep;
	pDelete = pParent;
	pKeep = parent->second;
	if (pDelete->Cost(gamma, delta, rho).cost < 
	    pKeep->Cost(gamma, delta, rho).cost) {
	  MOBox *pTmp = pDelete;
	  pDelete = pKeep;
	  pKeep = pTmp;
	}
	delete pDelete;
	parent->second = pKeep;
      }
    }
  }
}
