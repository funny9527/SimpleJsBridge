var global = {
    main : function(a, b) {
        return 'js function callback <main> called ' + a + '  ' + b;
    },

    BridgeBatch: {
         TestJsModule : {
             function2 : function(p1, p2) {
                         var a = "js function <function2> called " + p1 + " " + p2;
                         return a;
                     },
             function1 : function() {
                         var a = "js function <function1> called";
                     },
             test : function(p1, p2, p3, p4, p5) {
                     var res = global.NativeModules.TestNativeModule.getData(p1, p2, p3, p4, p5, global.main);
                     return res;
                 },
         },
    }
}