require(["dojo","dijit","dijit/form/FilteringSelect", "dojo/data/ItemFileReadStore",
         "dojo/_base/event","dojox/fx","dojo/behavior","dojo/domReady!"],
        function(dojo,dijit,FilteringSelect, ItemFileReadStore, event,fx, behavior) {

            // create store instance
            // referencing data from states.json
            var stateStore = new ItemFileReadStore({
                url: "/nutrition/macronutrients"
            });

            // create Select widget,
            // populating its options from the store
            var select = new FilteringSelect({
                name: "macronutrient_select",
                store: stateStore,
                required:false,
            }, "macronutrient_select");
            select.startup();

            function onAddMacroNutrient(){
                var macronutrient = select.get('value');
                stateStore.fetchItemByIdentity({identity:macronutrient,onItem:function(item){
                    console.log(item);
                    if(item){
                        var dosage = item.dosage[0];
                        addMacroNutrient(macronutrient, dosage);
                    }
                }});
            };

            function addMacroNutrient(macronutrient, dosage){
                var macronutrients_form = dojo.byId("macronutrients_form");
                var nutrients_found = dojo.query("input[name='"+macronutrient+"']");
                if(nutrients_found.length>0){
                    fx.highlight({node:nutrients_found[0]}).play();
                    fx.highlight({node:nutrients_found[0].parentNode}).play();
                }else{
                    createNutrientItem(macronutrient,dosage);
                }

            }
            function createNutrientItem(macronutrient,dosage){
                var closebtn = dojo.query(".closebtn")[0];
                var item = dojo.create('span',{
                    'class':'nutrient'},
                                       "macronutrients_form",'last');
                var myTextBox = new dijit.form.TextBox({
                    name: macronutrient,
                    size:"50",
                    value: dosage /* no or empty value! */,
                    /*placeHolder: "type in your name"*/
                });
                function setDrvPercent(e){
                    var drv_percent = calculateDRVPercent(myTextBox.get('name'),myTextBox.get('value'));
                    dojo.html.set(dojo.query('.drv_percent',item)[0],drv_percent+"% of drv*");
                }
                dojo.connect(myTextBox,'onChange',setDrvPercent);
                dojo.create('label',{innerHTML:macronutrient+" (grams): "}, item,'last');
                dojo.create("br",{},item,'last');
                dojo.place(myTextBox.domNode,item,'last');
                //dojo.create('input',{type:"text",name:macronutrient,value:dosage,size:"50"},item,'last');
                dojo.create('span',{'class':'drv_percent'},item,'last');
                dojo.create('img',{src:closebtn.src,'class':'closebtn'},item,'last');
                dojo.query(".closebtn",item).on("click", onClose);
                setDrvPercent(dosage);

            }
            function calculateDRVPercent(nutrient, grams){
                var drv = parseFloat(drv_map[nutrient]);
                var drv_percent = (grams/drv)*100;
                drv_percent = (Math.round((drv_percent*Math.pow(10,2)).toFixed(1))/Math.pow(10,2)).toFixed(2);
                return drv_percent;
            }
                
            function onClose(e){
                dojo.destroy(e.target.parentNode);
            }

                
            dojo.query(".form_btn").on("click", function(e) {
                try{
                    onAddMacroNutrient();
                }catch(err){
                    console.log(err);
                }
                event.stop(e);
                return false;
            });

            dojo.query(".closebtn").on("click", onClose);
            dojo.query(".submit").on("submit", function(){
                if(this.validate()){
                    return true;
                }else{
                    alert('Form contains invalid data.  Please correct first');
                    return false;
                }
                return true;
            });
            dojo.forEach(nutrient_map,function(item,i){
                createNutrientItem(item[0],item[1]);
                });
    });
