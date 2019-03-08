import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class DeviceService {

  constructor(public http: HttpClient) { }

  getCurrentPosition( baseUrl ): Observable<any>{
    return this.http.get( baseUrl);
  }

}
