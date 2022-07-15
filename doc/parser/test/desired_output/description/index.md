# <a name='description' />  description

namespace for everything that can go wrong in the doxygen description inside the source code, that is not handled anywhere else 




## Functions
| Name | Description | 
| ---- | ---- |
| [function_with_little_code](#description-function_with_little_code) | This description contains `bits of code`  |
| [function_with_code_block](#description-function_with_code_block) | This description contains more code.  |
| [function_with_code_text_mix](#description-function_with_code_text_mix) | This description contains code mixed in with text.  |
| [function_with_remark](#description-function_with_remark) | this is a function with a  |
| [function_with_pre_condition](#description-function_with_pre_condition) | this is a function with a  |
| [function_with_post_condition](#description-function_with_post_condition) | this is a function with a  |
| [function_with_warning](#description-function_with_warning) | this is a function with a  |
| [function_with_bold_text](#description-function_with_bold_text) | this is a function with a **bold** word  |
| [function_with_italic_text](#description-function_with_italic_text) | this is a function with *italic*, *italic*, or *italic* text  |
| [function_with_long_description](#description-function_with_long_description) | this is a function with a short brief  |
| [function_with_list](#description-function_with_list) | this description has a  |
| [function_with_todo](#description-function_with_todo) | this is a function with a  |
| [function_with_todo_and_reference](#description-function_with_todo_and_reference) | this is a function with a  |
| [function_with_todo_and_code](#description-function_with_todo_and_code) | this is a function with a  |
| [function_with_two_todos](#description-function_with_two_todos) | this is a function with one  |



## Functions
### <a name='description-function_with_little_code' /> public void description::function_with_little_code ()

This description contains `bits of code` 










These can also be with a single `letter` 




[Go to Top](#description)

### <a name='description-function_with_code_block' /> public void description::function_with_code_block ()

This description contains more code. 











```c++
for(int i=0; i < size; ++i)
{
    do_something(i);
}
```

 




[Go to Top](#description)

### <a name='description-function_with_code_text_mix' /> public void description::function_with_code_text_mix ()

This description contains code mixed in with text. 










Starting with an introduction...

We have some text before the code block 
```c++
for(int i=0; i < size; ++i)
{
    do_something(i);
}
```

 and directly after.

And more text to make things interesting in the end 




[Go to Top](#description)

### <a name='description-function_with_remark' /> public void description::function_with_remark ()

this is a function with a 










> **remark:** remarking important things 




[Go to Top](#description)

### <a name='description-function_with_todo' /> public void description::function_with_todo ()

this is a function with a 










> **[Todo][todo]:** to remember important things 




[Go to Top](#description)

### <a name='description-function_with_todo_and_reference' /> public void description::function_with_todo_and_reference ()

this is a function with a 










> **[Todo][todo]:** that contains a [function_with_todo][description-function_with_todo] 




[Go to Top](#description)

### <a name='description-function_with_todo_and_code' /> public void description::function_with_todo_and_code ()

this is a function with a 










> **[Todo][todo]:** that contains a `bit of code` 




[Go to Top](#description)

### <a name='description-function_with_two_todos' /> public void description::function_with_two_todos ()

this is a function with one 










> **[Todo][todo]:** and also another

> **[Todo][todo]:** to test for multiple Todos 




[Go to Top](#description)

### <a name='description-function_with_pre_condition' /> public void description::function_with_pre_condition ()

this is a function with a 










> **pre:** describing conditions 




[Go to Top](#description)

### <a name='description-function_with_post_condition' /> public void description::function_with_post_condition ()

this is a function with a 










> **post:** describing conditions 




[Go to Top](#description)

### <a name='description-function_with_warning' /> public void description::function_with_warning ()

this is a function with a 










> **warning:** of pitfalls 




[Go to Top](#description)

### <a name='description-function_with_bold_text' /> public void description::function_with_bold_text ()

this is a function with a **bold** word 










Or **more bold text** without and with **spaces** 




[Go to Top](#description)

### <a name='description-function_with_italic_text' /> public void description::function_with_italic_text ()

this is a function with *italic*, *italic*, or *italic* text 








[Go to Top](#description)

### <a name='description-function_with_long_description' /> public void description::function_with_long_description ()

this is a function with a short brief 










And a really long description with first a details command ..... ............................................................... ............................................................... ...............................................................

............................................................... ...............................................................

And then without, just as pain text .................................... ........................................................................ ........................................................................

........................................................................ ........................................................................ 




[Go to Top](#description)

### <a name='description-function_with_list' /> public void description::function_with_list ()

this description has a 










list of items:
- one 

- two 

- five 

Three Sir! Three! Boom! 




[Go to Top](#description)

[description-function_with_todo]:#description-function_with_todo
[todo]:./../todo.md#todo
