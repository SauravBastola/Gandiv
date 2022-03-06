<program> -> <program> <statement>
<program> -> <statement>

<statement> -> <expression> ;
<statement> -> <assignment> ;
<statement> -> <print_statement> ;
<statement> -> <return_statement> ;
<statement> -> <if_statement>
<statement> -> <while_statement>
<statement> -> <for_statement>
<statement> -> <simple_for_statement>
<statement> -> <function_decl>
<statement> -> <break_statement> ;
<statement> -> <continue_statement> ;

<assignment> -> identifier = <expression>

<print_statement> -> print ( <expressions> )
<expressions> -> <expression> , <expressions>
<expressions> -> endl , <expressions>
<expressions> -> <expression>
<expressions> -> endl


<return_statement> -> return <expression>
<return_statement> -> return
<break_statement> -> break
<continue_statement> -> continue

<function_decl> = def identifier ( <parameters> ) { <program> }
<function_decl> = def identifier ( ) { <program> }
<parameters> = <parameters> , identifier
<parameters> = identifier


<function_call> -> identifier ( <parameter_values> )
<function_call> -> identifier ( )
<parameter_values> -> <parameter_values> , <expression>
<parameter_values> -> <expression>


<expression> -> <expression> or <anded_boolean>
<expression> -> <anded_boolean>
<anded_boolean> -> <anded_boolean> and <boolean_value>
<anded_boolean> -> <boolean_value>
<boolean_value> -> ( <expression> )
<boolean_value> -> <arithmetic>   ==   <arithmetic>
<boolean_value> -> <arithmetic>   !=   <arithmetic>
<boolean_value> -> <arithmetic>   <    <arithmetic>
<boolean_value> -> <arithmetic>   >    <arithmetic>
<boolean_value> -> <arithmetic>   <=   <arithmetic>
<boolean_value> -> <arithmetic>   >=   <arithmetic>
<boolean_value> -> <arithmetic>


<arithmetic> -> <arithmetic> + <term>
<arithmetic> -> <arithmetic> - <term>
<arithmetic> -> <term>

<term> -> <term> * <factor>
<term> -> <term> / <factor>
<term> -> <term> // <factor>
<term> -> <term> % <factor>
<term> -> <factor>

<factor> -> <literal>
<factor> -> identifier
<factor> -> ( <arithmetic> )
<factor> -> <function_call>
<factor> -> input ( )

<literal> -> int_literal
<literal> -> real_literal
<literal> -> <bool_literal>
<literal> -> string_literal

<bool_literal> -> true
<bool_literal> -> false


<if_statement> -> if ( <expression> ) { <program> }
<if_statement> -> if ( <expression> ) { <program> } else { <program> }
<while_statement> -> while ( <expression> ) { <program> }
<for_statement> -> for ( <assignment> ; <expression> ; <assignment> ) { <program> }
<simple_for_statement> -> for identifier = <expression> to <expression> { <program> }
