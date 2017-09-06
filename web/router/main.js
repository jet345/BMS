module.exports = function(app)
{
     app.get('/1st_floor',function(req,res){
        res.render('index.html')
     });
}