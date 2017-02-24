# SimpleReact
a js bridge for android
一个react-native的简化版本，去掉了ui渲染部分，紧紧实现了jsBridge部分，实现了从javascript到c++再到android(java)之间的调用，同时也可以反过来调用。核心库使用javascriptcore,bridge的主要方法：JSObjectCallAsFunction，JSObjectSetProperty，JSClassDefinition,JSObjectGetProperty.
