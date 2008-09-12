#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "smalloc.h"
#include "gmx_fatal.h"
#include "molprop.h"
	
typedef struct {
  int    eMP;
  char   *pname;
  double pvalue,perror;
  char   *pmethod;
  char   *preference;
} t_property;

typedef struct {
  char *cname;
  int  cnumber;
} t_catom;

typedef struct {
  char    *compname;
  int     ncatom,ncatom_c;
  t_catom *catom;
} t_composition;

typedef struct {
  char          *formula,*molname,*reference;
  double        weight;
  int           nproperty,nprop_c;
  t_property    *property;
  int           ncomposition,ncomp_c;
  t_composition *composition;
  int           ncategory,ncateg_c;
  char          **category;
} gmx_molprop;

gmx_molprop_t gmx_molprop_init()
{
  gmx_molprop *mp;
  
  snew(mp,1);
  
  return (gmx_molprop_t) mp;
}

void gmx_molprop_delete(gmx_molprop_t mpt)
{
  fprintf(stderr,"gmx_molprop_delete not implemented yet\n");
}

void gmx_molprop_set_weight(gmx_molprop_t mpt,double weight)
{
  gmx_molprop *mp = (gmx_molprop *) mpt;
  
  mp->weight=weight;
}

double gmx_molprop_get_weight(gmx_molprop_t mpt)
{
  gmx_molprop *mp = (gmx_molprop *) mpt;
  
  return mp->weight;
}

void gmx_molprop_set_formula(gmx_molprop_t mpt,char *formula)
{
  gmx_molprop *mp = (gmx_molprop *) mpt;
  
  if (mp->formula != NULL)
    sfree(mp->formula);
  mp->formula = strdup(formula);
}

char *gmx_molprop_get_formula(gmx_molprop_t mpt)
{
  gmx_molprop *mp = (gmx_molprop *) mpt;
  
  return mp->formula;
}

void gmx_molprop_set_molname(gmx_molprop_t mpt,char *molname)
{
  gmx_molprop *mp = (gmx_molprop *) mpt;
  
  if (mp->molname != NULL)
    sfree(mp->molname);
  mp->molname = strdup(molname);
}

char *gmx_molprop_get_molname(gmx_molprop_t mpt)
{
  gmx_molprop *mp = (gmx_molprop *) mpt;
  
  return mp->molname;
}

void gmx_molprop_set_reference(gmx_molprop_t mpt,char *reference)
{
  gmx_molprop *mp = (gmx_molprop *) mpt;
  
  if (mp->reference != NULL)
    sfree(mp->reference);
  mp->reference = strdup(reference);
}

char *gmx_molprop_get_reference(gmx_molprop_t mpt)
{
  gmx_molprop *mp = (gmx_molprop *) mpt;
  
  return mp->reference;
}

int gmx_molprop_add_composition(gmx_molprop_t mpt,char *compname)
{
  gmx_molprop *mp = (gmx_molprop *) mpt;
  int i,j;
  
  for(i=0; (i<mp->ncomposition); i++)
    if(strcasecmp(compname,mp->composition[i].compname) == 0)
      break;
  if (i == mp->ncomposition) {
    mp->ncomposition++;
    srenew(mp->composition,mp->ncomposition);
    mp->composition[i].compname = strdup(compname);
    mp->composition[i].ncatom = 0;
    mp->composition[i].ncatom_c = 0;
    mp->composition[i].catom = NULL;
  }
}


void gmx_molprop_delete_composition(gmx_molprop_t mpt,char *compname)
{
  gmx_molprop *mp = (gmx_molprop *) mpt;
  int i,j;
  
  for(i=0; (i<mp->ncomposition); i++)
    if(strcasecmp(compname,mp->composition[i].compname) == 0)
      break;
  if (i < mp->ncomposition) {
    for(j=0; (j<mp->composition[i].ncatom); j++)
      sfree(mp->composition[i].catom[j].cname);
    sfree(mp->composition[i].catom);
    mp->composition[i].ncatom = 0;
  }  
}

char *gmx_molprop_get_composition(gmx_molprop_t mpt)
{
  gmx_molprop *mp = (gmx_molprop *) mpt;

  if (mp->ncomp_c < mp->ncomposition) {
    return mp->composition[mp->ncomp_c].compname;
  }
  mp->ncomp_c = 0;
  return NULL;
}

int gmx_molprop_count_composition_atoms(gmx_molprop_t mpt,
					char *compname,char *atom)
{
  gmx_molprop *mp = (gmx_molprop *) mpt;
  int i,j;
  
  for(i=0; (i<mp->ncomposition); i++)
    if(strcasecmp(compname,mp->composition[i].compname) == 0)
      break;
  if (i < mp->ncomposition) {
    for(j=0; (j<mp->composition[i].ncatom); j++)
      if (strcasecmp(mp->composition[i].catom[j].cname,atom) == 0)
	return mp->composition[i].ncatom;
  }  
  return 0;
}
					       
void gmx_molprop_add_composition_atom(gmx_molprop_t mpt,char *compname,
				      char *atomname,int natom)
{
  gmx_molprop *mp = (gmx_molprop *) mpt;
  int i=-1,j;
  
  if (compname) {
    for(i=0; (i<mp->ncomposition); i++)
      if(strcasecmp(compname,mp->composition[i].compname) == 0)
	break;
    if (i == mp->ncomposition) {
      mp->ncomposition++;
      srenew(mp->composition,mp->ncomposition);
      mp->composition[i].compname = strdup(compname);
      mp->composition[i].ncatom = 0;
      mp->composition[i].catom = NULL;
    }
  }
  else if (mp->ncomposition > 0) 
    i = mp->ncomposition-1;
    
  if (i >= 0) {
    for(j=0; (j<mp->composition[i].ncatom); j++) {
      if (strcasecmp(mp->composition[i].catom[j].cname,atomname) == 0) {
	mp->composition[i].catom[j].cnumber += natom;
	break;
      }
    }
    if (j == mp->composition[i].ncatom) {
      mp->composition[i].ncatom++;
      srenew(mp->composition[i].catom,mp->composition[i].ncatom);
      mp->composition[i].catom[j].cnumber = 0;
      mp->composition[i].catom[j].cname = strdup(atomname);
    }
    mp->composition[i].catom[j].cnumber += natom;
  }
  else
    gmx_incons("gmx_molprop_add_composition_atom called with invalid arguments");
}

int gmx_molprop_get_composition_atom(gmx_molprop_t mpt,
				     char **atomname,int *natom)
{
  gmx_molprop *mp = (gmx_molprop *) mpt;
  t_composition *cc;
  int nc;
  
  if (mp->ncomp_c < mp->ncomposition) {
    cc = &(mp->composition[mp->ncomp_c]);
    nc = cc->ncatom_c;
    if (nc < cc->ncatom) {
      *atomname = strdup(cc->catom[nc].cname);
      *natom = cc->catom[nc].cnumber;
      cc->ncatom_c++;
      return 1;
    }
    cc->ncatom_c = 0;
    mp->ncomp_c++;
  }
 
  return 0;
}

void gmx_molprop_add_property(gmx_molprop_t mpt,int eMP,
			      char *prop_name,double value,double error,
			      char *prop_method,char *prop_reference)
{
  gmx_molprop *mp = (gmx_molprop *) mpt;
  t_property  *pp;
  
  mp->nproperty++;
  srenew(mp->property,mp->nproperty);
  pp = &(mp->property[mp->nproperty-1]);
  pp->eMP        = eMP;
  pp->pname      = strdup(prop_name);
  pp->pvalue     = value;
  pp->perror     = error;
  pp->pmethod    = strdup(prop_method);
  pp->preference = strdup(prop_reference);
}

int gmx_molprop_get_property(gmx_molprop_t mpt,int *eMP,
			     char **prop_name,double *value,double *error,
			     char **prop_method,char **prop_reference)
{
  gmx_molprop *mp = (gmx_molprop *) mpt;
  t_property  *pp;
  
  if (mp->nprop_c < mp->nproperty) {
    mp->nprop_c++;
    pp = &(mp->property[mp->nprop_c-1]);
    *eMP       = pp->eMP;       
    *prop_name = strdup(pp->pname);
    *value     = pp->pvalue;
    *error     = pp->perror;
    *prop_method = strdup(pp->pmethod);
    *prop_reference = strdup(pp->preference);

    return 1;
  }
  mp->nprop_c = 0;
  return 0;
}

int gmx_molprop_search_property(gmx_molprop_t mpt,int eMP,
				char *prop_name,double *value,double *error,
				char *prop_method,char **prop_reference)
{
  gmx_molprop *mp = (gmx_molprop *) mpt;
  t_property  *pp;
  int i;
  
  for(i=0; (i< mp->nproperty); i++) {
    pp = &(mp->property[i]);
    if (((eMP == eMOLPROP_Any) || (eMP == pp->eMP)) && 
	(strcasecmp(prop_name,pp->pname) == 0) &&
	((prop_method != NULL) && (strcasecmp(prop_method,pp->pmethod) == 0))) {
      *value     = pp->pvalue;
      *error     = pp->perror;
      *prop_reference = strdup(pp->preference);

      return 1;
    }
  }
  return 0;
}

void gmx_molprop_add_category(gmx_molprop_t mpt,char *category)
{
  gmx_molprop *mp = (gmx_molprop *) mpt;
  int i;
  
  for(i=0; (i<mp->ncategory); i++) 
    if (strcasecmp(mp->category[i],category) == 0)
      break;
  if (i == mp->ncategory) {
    mp->ncategory++;
    srenew(mp->category,mp->ncategory);
    mp->category[i] = strdup(category);
  }
} 

void gmx_molprop_reset_category(gmx_molprop_t mpt)
{
  gmx_molprop *mp = (gmx_molprop *) mpt;

  mp->ncateg_c = 0;
}

char *gmx_molprop_get_category(gmx_molprop_t mpt)
{
  gmx_molprop *mp = (gmx_molprop *) mpt;
  int i;
  
  if (mp->ncateg_c < mp->ncategory) {
    mp->ncateg_c++;
    return mp->category[mp->ncateg_c-1];
  }
  gmx_molprop_reset_category(mpt);
  
  return NULL;
}

int gmx_molprop_search_category(gmx_molprop_t mpt,char *catname)
{
  gmx_molprop *mp = (gmx_molprop *) mpt;
  int i;
  
  for(i=0; (i<mp->ncategory); i++)
    if (strcasecmp(mp->category[i],catname) == 0)
      return 1;
  
  return 0;
}

void gmx_molprop_merge(gmx_molprop_t dst,gmx_molprop_t src)
{
  int i,nd,ns,eMP;
  gmx_molprop *ddd = (gmx_molprop *)dst;
  char *tmp,*prop_name,*prop_method,*prop_reference,*catom;
  int cnumber;
  double value,error;
  
  while ((tmp = gmx_molprop_get_category(src)) != NULL) {
    gmx_molprop_add_category(dst,tmp);
  }
	
  while (gmx_molprop_get_property(src,&eMP,&prop_name,&value,&error,
				  &prop_method,&prop_reference) == 1) {
    gmx_molprop_add_property(dst,eMP,prop_name,value,error,
			     prop_method,prop_reference);
    sfree(prop_name);
    sfree(prop_method);
    sfree(prop_reference);
  }

  if (ddd->ncomposition == 0) {
    while ((tmp = gmx_molprop_get_composition(src)) != NULL) {
      gmx_molprop_add_composition(dst,tmp);
      while ((gmx_molprop_get_composition_atom(src,&catom,&cnumber)) == 1) {
	gmx_molprop_add_composition_atom(dst,tmp,catom,cnumber);
	sfree(catom);
      }
    }
  }
  else {
    printf("Both src and dst for %s contain composition entries. Not changing anything\n",ddd->molname);
  }
}

gmx_molprop_t gmx_molprop_copy(gmx_molprop_t mpt)
{
  gmx_molprop_t dst = gmx_molprop_init();
  
  gmx_molprop_set_molname(dst,gmx_molprop_get_molname(mpt));
  gmx_molprop_set_formula(dst,gmx_molprop_get_formula(mpt));
  gmx_molprop_set_weight(dst,gmx_molprop_get_weight(mpt));
  gmx_molprop_set_reference(dst,gmx_molprop_get_reference(mpt));

  gmx_molprop_merge(dst,mpt);
  
  return dst;
}

