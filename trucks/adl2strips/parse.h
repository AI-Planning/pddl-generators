


/*
 * THIS SOURCE CODE IS SUPPLIED  ``AS IS'' WITHOUT WARRANTY OF ANY KIND, 
 * AND ITS AUTHOR AND THE JOURNAL OF ARTIFICIAL INTELLIGENCE RESEARCH 
 * (JAIR) AND JAIR'S PUBLISHERS AND DISTRIBUTORS, DISCLAIM ANY AND ALL 
 * WARRANTIES, INCLUDING BUT NOT LIMITED TO ANY IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, AND
 * ANY WARRANTIES OR NON INFRINGEMENT.  THE USER ASSUMES ALL LIABILITY AND
 * RESPONSIBILITY FOR USE OF THIS SOURCE CODE, AND NEITHER THE AUTHOR NOR
 * JAIR, NOR JAIR'S PUBLISHERS AND DISTRIBUTORS, WILL BE LIABLE FOR 
 * DAMAGES OF ANY KIND RESULTING FROM ITS USE.  Without limiting the 
 * generality of the foregoing, neither the author, nor JAIR, nor JAIR's
 * publishers and distributors, warrant that the Source Code will be 
 * error-free, will operate without interruption, or will meet the needs 
 * of the user.
 */





/*********************************************************************
 * File: parse.h
 * Description: Functions for the pddl parser
 *
 * Author: Frank Rittinger 1998 / Joerg Hoffmann 1999
 *
 *********************************************************************/ 





#ifndef _PARSE_H
#define _PARSE_H



char *copy_Token( char *s );
TokenList *copy_TokenList( TokenList *source );
void strupcase( char *from );
char *rmdash( char *s );



void build_orig_constant_list( void );
void collect_type_names_in_pl( PlNode *n );
int get_type( char *str );
void make_either_ty( TypedList *tyl );
void make_either_ty_in_pl( PlNode *n );
void normalize_tyl_in_pl( PlNode **n );



Bool make_adl_domain( void );
Bool make_conjunction_of_atoms( PlNode **n );
Bool is_wff( PlNode *n );
Bool make_effects( PlNode **n );
Bool is_eff_literal( PlNode *n );
Bool make_conjunction_of_literals( PlNode **n );



#endif /* PARSE */
