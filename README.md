# JSX translator
## How To Use?
first download **html.h** , and import it
```cpp
#include "html.h"
```
## What Can I Do With It?
### parse html code
you can easily parse html code
```cpp
vector<maker::dom> parsedData = html::main::parse(code);
```
### make jsx code
you can easily make jsx code
```cpp
setting set;
set.domMaker = "Var.dom";
set.textMaker = "Var.text";
set.stateMaker = "Var.state";
set.parentTag = "jsx";

cout << main::make(code,set) << endl;
```
`domMaker` is the function's name that makes virtual dom
the function must look like this
```js
const funcName = (tagName: string, states: Array<stateClass>, ...kids:domClass) => {...}
```
`stateMaker` is the function's name that makes virtual state
the function must look like this
```js
const funcName = (stateName: string, stateVal: string) => {...}
```
`textMaker` is the function's name that makes virtual text
the function must look like this
```js
const funcName = (value: string) => {...}
```
`parentTag` is the tag name the jsx code's first dom
like this
```html
<parentTag>
  <!--your doms-->
</parentTag>
```
