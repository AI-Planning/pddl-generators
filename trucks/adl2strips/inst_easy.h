


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
 * File: inst_easy.h
 * Description: headers for multiplying easy operators.
 *
 *
 * Author: Joerg Hoffmann 2000
 *
 *********************************************************************/ 








#ifndef _INST_EASY_H
#define _INST_EASY_H



void build_easy_action_templates( void );



void cleanup_easy_domain( void );
Bool identical_fact( Fact *f1, Fact *f2 );
void remove_unused_easy_parameters( void );
void replace_var_entries( NormOperator *o, int p0, int p1 );
void decrement_var_entries( NormOperator *o, int start );
void remove_unused_easy_effect_parameters( NormOperator *o, NormEffect *e );
void handle_empty_easy_parameters( void );



void encode_easy_unaries_as_types( void );
int create_intersected_type( TypeArray T, int num_T );
int find_intersected_type( TypeArray T, int num_T );



void multiply_easy_effect_parameters( void );
void unify_easy_inertia_conditions( int curr_inertia );
void multiply_easy_non_constrained_effect_parameters( int curr_parameter );



void multiply_easy_op_parameters( void );
void unify_easy_inertia_preconds( int curr_inertia );
void multiply_easy_non_constrained_op_parameters( int curr_parameter );



#endif /* _INST_EASY_H */
