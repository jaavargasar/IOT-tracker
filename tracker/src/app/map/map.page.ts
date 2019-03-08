import { Component, OnInit } from '@angular/core';
import {Platform, NavController} from '@ionic/angular';
import { ServerConfig } from '../../../config/server';
import {
  GoogleMaps, 
  GoogleMap, 
  Environment, 
  GoogleMapsEvent, 
  MyLocation, 
  GoogleMapsAnimation,
  Marker,
 } from '@ionic-native/google-maps/ngx';
import { ToastController } from '@ionic/angular';
import { DeviceService } from 'src/services/device.service';
import { ILatLng } from '@ionic-native/google-maps';
import { HomePage } from '../home/home.page';

@Component({
  selector: 'app-map',
  templateUrl: './map.page.html',
  styleUrls: ['./map.page.scss'],
})
export class MapPage implements OnInit {

  map: GoogleMap;
  mapReady: boolean = false;
  protected mapApi = ServerConfig.mapApiKey;
  protected api = ServerConfig.endPoint;
  curPos = { latitude: null, longitude: null};
  curLocation = { lat: null, lng: null};
  interval: any;
  curInterval: any;
  curMarker = null;
  curLocationMarker = null;

  oldMarker = null;
  oldLocationMarker = null;

  constructor(
    private platform: Platform, 
    public toastController: ToastController,
    private deviceService: DeviceService,
    public navCtrl: NavController) { }

  async ngOnInit() {
    await this.platform.ready();
    await this.loadMap();

    
    this.curInterval = setInterval( async ()=>{
       await this.getCurrentLocation().then(()=>{
        this.oldMarker.remove();
       });
    }, 5000);

    this.interval = setInterval( async () => {
      await this.doHttpRequest().then(()=>{
        this.oldLocationMarker.remove();
      });
     }, 5000);
    
  }

  async doHttpRequest(){
    const oldPosition = this.curPos;
    await this.deviceService.getCurrentPosition(`${this.api}/device`)
      .subscribe(
        (response) => {
          this.curPos = response;
          if( this.checkPositionChanged( this.curPos, oldPosition) ){ //theres a new marker

            let position: ILatLng = {
              lat: this.curPos.latitude,
              lng: this.curPos.longitude
            };

            console.log( { latLng: position} );

            if(this.curMarker){
              this.oldMarker = this.curMarker;
              this.curMarker.remove();
            }

            return this.addNewMarker( { latLng: position},'blue','device location', this.curMarker )
            .then((marker: Marker) => {

              this.curMarker = marker;
              // show the infoWindow
              marker.showInfoWindow();
      
              // If clicked it, display the alert
              marker.on(GoogleMapsEvent.MARKER_CLICK).subscribe(() => {
                this.presentToast('clicked!');
              });
            });

          }
        },
        (error) => console.log(error)
      );
  }

  checkPositionChanged( ob1, ob2){
    if( !ob1.latitude ) return false;
    if( !ob1.longitude) return false;
    if( ob1.latitude!== ob2.latitude || ob1.longitude!== ob2.longitude) return true;
    return false;
  }

  async getCurrentLocation() {
    // Get the location of you
    const oldLocation = this.curLocation;
    await this.map.getMyLocation()
      .then((location: MyLocation) => {
        // console.log(JSON.stringify(location, null ,2));

        this.curLocation = location.latLng;
        
        var result = null;
        if( this.checkPositionChanged( 
          { "latitude": this.curLocation.lat , "longitude": this.curLocation.lng} ,
          { "latitude": oldLocation.lat, "longitude": oldLocation.lng} ) ){
   
          if( this.curLocationMarker ){
            this.oldLocationMarker = this.curLocationMarker;
            this.curLocationMarker.remove();
          }

          

          result  =  this.addNewMarker( { latLng: this.curLocation},'red','current location', this.curLocationMarker )
            .then((marker: Marker) => {

              this.curLocationMarker = marker;
              // show the infoWindow
              marker.showInfoWindow();
      
              // If clicked it, display the alert
              marker.on(GoogleMapsEvent.MARKER_CLICK).subscribe(() => {
                this.presentToast('clicked!');
              });
         });
        }

        return result;

      })
  }

  addNewMarker(location,color, _title , _marker){

    return this.map.animateCamera({
      target: location.latLng,
      zoom: 17, //17
      tilt: 30
    }).then(() => {
      // add a marker
      _marker = this.map.addMarker({
        title: _title,
        position: location.latLng,
        animation: GoogleMapsAnimation.BOUNCE,
        icon: color
      });

      return _marker;
    })
  }

  loadMap(){
    Environment.setEnv({
      'API_KEY_FOR_BROWSER_RELEASE': `${this.mapApi}`,
      'API_KEY_FOR_BROWSER_DEBUG': `${this.mapApi}`
    });

    this.map = GoogleMaps.create('map_canvas');
    this.map.one(GoogleMapsEvent.MAP_READY).then(() => {
      this.mapReady = true;
    });
  }

  async presentToast( inMessage) {
    const toast = await this.toastController.create({
      message: inMessage,
      duration: 2000
    });
    toast.present();
  }


}
