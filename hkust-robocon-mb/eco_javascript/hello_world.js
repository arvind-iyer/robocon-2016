/**
 * Created by Budi Ryan on 2/28/2016.
 */
var Cylon = require('cylon');

Cylon.robot({
    work: function() {
        every((0.01).second(), function() {
            console.log("Hello, human!");
        });
    }
}).start();