grammar selector_list_1;

selector_list_1 : complex_selector_list;

//complex_selector_list : complex_selector#;
complex_selector_list : complex_selector (',' complex_selector)?;

	
//compound_selector_list : compound_selector#;
compound_selector_list : compound_selector (',' compound_selector)?;

//simple_selector_list : simple_selector#;
simple_selector_list : simple_selector (',' simple_selector)?;

//relative_selector_list : relative_selector#;
relative_selector_list : relative_selector (',' relative_selector)?;


complex_selector : compound_selector ( combinator? compound_selector )*;

relative_selector : combinator? complex_selector;

//compound_selector : ( type_selector? subclass_selector*
//                        ( pseudo_element_selector pseudo_class_selector* )* )!;

compound_selector : compound_selector_1
                  | compound_selector_2
                  | compound_selector_3
                  ;

compound_selector_1 : type_selector subclass_selector*
                        ( pseudo_element_selector pseudo_class_selector* )*;

compound_selector_2 : subclass_selector+
                        ( pseudo_element_selector pseudo_class_selector* )* ;

compound_selector_3 : ( pseudo_element_selector pseudo_class_selector* )+;
                        
simple_selector : type_selector
                | subclass_selector
                ;


combinator : '>'
           | '+'
           | '~'
           | ( '|' '|' )
           ;

type_selector : wq_name
              | ns_prefix? '*';

ns_prefix : ( IDENT_TOKEN | '*' )? '|';

wq_name : ns_prefix? IDENT_TOKEN;

subclass_selector : id_selector
                  | class_selector
                  | attribute_selector
                  | pseudo_class_selector
                  ;

id_selector : HASH_TOKEN;

class_selector : '.' IDENT_TOKEN;

attribute_selector : '[' wq_name ']'
                   | '[' wq_name attr_matcher ( STRING_TOKEN | IDENT_TOKEN ) attr_modifier? ']';

attr_matcher : ( '~' | '|' | '^' | '$' | '*' )? '=';

attr_modifier : 'i' | 'I' | 's' | 'S';

pseudo_class_selector : ':' IDENT_TOKEN
                      | ':' FUNCTION_TOKEN ANY_VALUE ')'
                      ;

pseudo_element_selector : ':' pseudo_class_selector
                        ;

// token

HASH_TOKEN : '#' ('a'..'z' | 'A'..'Z')+;

//IDENT_TOKEN : ('a'..'z' | 'A'..'Z')+;
IDENT_TOKEN : ('a'..'z' | 'A'..'Z')+;
STRING_TOKEN : IDENT_TOKEN;

FUNCTION_TOKEN : IDENT_TOKEN '(' STRING_TOKEN ')';

ANY_VALUE : IDENT_TOKEN;







