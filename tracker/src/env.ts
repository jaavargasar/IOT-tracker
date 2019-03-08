interface EnvironmentInterface {
    mode: string;
    MAP_API_KEY: string;
    endPoint: string;
  }
  
export const ENV: EnvironmentInterface = {
    /** DEV */
    mode: 'dev',
    MAP_API_KEY: 'AIzaSyDezAgic-LJqrslTHQH99HWI2QJf-EDmGg',
    endPoint: 'http://172.20.10.3:10010'
};
  