grammar selector_list3;

selector_list3 : complex_selector_list;

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

ns_prefix : ( STRING_TOKEN | '*' )? '|';

wq_name : ns_prefix? STRING_TOKEN;

subclass_selector : id_selector
                  | class_selector
                  | attribute_selector
                  | pseudo_class_selector
                  ;

id_selector : STRING_TOKEN;

class_selector : '.' STRING_TOKEN;

attribute_selector : '[' wq_name ']'
                   | '[' wq_name attr_matcher ( STRING_TOKEN | ident_token ) attr_modifier? ']';

attr_matcher : ( '~' | '|' | '^' | '$' | '*' )? '=';

attr_modifier : 'i' | 'I' | 's' | 'S';

pseudo_class_selector : ':' STRING_TOKEN
                      | ':' STRING_TOKEN STRING_TOKEN ')'
                      ;

pseudo_element_selector : ':' pseudo_class_selector
                        ;

// token

ident_token : STRING_TOKEN;
STRING_TOKEN : '#' ('a'..'z' | 'A'..'Z' | '0'..'9')+;


