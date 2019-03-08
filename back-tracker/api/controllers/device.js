'use strict';


var curPos = {
    latitude: null,
    longitude: null
};

function postPosition( req, res){
    const data = req.body;
    console.log( `before: ${curPos.latitude} - ${curPos.longitude}\n`);
    curPos = {
        latitude: data.latitude,
        longitude: data.longitude
    }
    console.log( `after: ${curPos.latitude} - ${curPos.longitude}\n`);

    res.status(201).send( { message: 'success!' } );
    
}

function getPosition( req, res){
    res.status( 200).send( curPos );
}

module.exports = {
    postPosition,
    getPosition
}