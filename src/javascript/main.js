/* Author:

*/

function deletepost(postid){
    $.getJSON("/deletepost/"+postid+"/",function(data, status){
        console.log(status);
        console.log(data);
        if(status == 'success'){
            $('#'+data.id).remove();
        }
    });
};
