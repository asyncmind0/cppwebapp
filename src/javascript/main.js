/* Author:

*/

require(["dojo","dojo/_base/event","dojo/parser","dijit/layout/BorderContainer",
         "dijit/layout/TabContainer", "dijit/layout/ContentPane",
         "dijit/form/Form","dijit/form/Button",
         "dijit/form/TextBox","dijit/form/SimpleTextarea"
        ], function(dojo,event, parser){
    function deletepost(e){
        var deferred = dojo.xhrGet({
            url:dojo.attr(e.target,'href'),
            handleAs:'json',
            load:function(data, ioargs){
                if(ioargs.xhr.status == 200){
                    dojo.destroy(data.id);
                }

            }
        });
        event.stop(e);
        return false;
    }
    dojo.query('.deletepost').on('click',deletepost);
    parser.parse();
    dojo.style(dojo.query('.content')[0],'display','');
    dojo.style('sidebar','display','');
});
