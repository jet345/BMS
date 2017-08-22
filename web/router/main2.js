module.exports = function(app)
{
     app.get('/2nd_floor',function(req,res){
        res.render('index2.html')
     });
}