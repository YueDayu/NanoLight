import React from 'react';

import {Button, Card, Form} from 'react-bootstrap';

import 'bootstrap/dist/css/bootstrap.min.css';
import './App.css';

class VISUAL_MODE {
  static DARK = 0;
  static NORMAL = 1;

  static not(mode) {
    return 1 - mode;
  }

  static str(mode) {
    switch (mode) {
      case this.DARK:
        return "红光模式";
      case this.NORMAL:
        return "正常模式";
      default:
        return "UNKNOWN";
    }
  }
}

class DIRECTION {
  static POS = 0;
  static NEG = 1;
  static STOP = 2;

  static str_clock(direction) {
    switch (direction) {
      case this.POS:
        return "逆时针";
      case this.NEG:
        return "顺时针";
      default:
        return "停止";
    }
  }

  static str(direction) {
    switch (direction) {
      case this.POS:
        return "北半球";
      case this.NEG:
        return "南半球";
      default:
        return "";
    }
  }
}

class SPEED_TYPE {
  static FACTOR = 0;
  static DEGREE = 1;
}

class Speed {
  static Stellar_speed = 15;

  constructor(type = SPEED_TYPE.FACTOR, speed = 0) {
    this.type = type;
    this.speed = speed;
  }

  str_num() {
    let output_speed = this.speed;
    if (this.type === SPEED_TYPE.FACTOR) {
      output_speed *= Speed.Stellar_speed;
    }
    if (output_speed > 360) {
      return output_speed / 60 + "°/min"
    }
    return output_speed + "°/h";
  }

  str() {
    if (this.speed === 0) {
      return "停止";
    }
    if (this.type === SPEED_TYPE.FACTOR) {
      return this.speed + "x";
    }
    return "";
  }
}

class App extends React.Component {
  static default_speed_list = [
    new Speed(SPEED_TYPE.FACTOR, 1),
    new Speed(SPEED_TYPE.FACTOR, 5),
    new Speed(SPEED_TYPE.FACTOR, 10),
    new Speed(SPEED_TYPE.FACTOR, 100)];

  constructor(props) {
    super(props);
    this.state = {
      // visual config
      visual_mode: VISUAL_MODE.NORMAL,
      // display info, fetch from server
      status_fetched: false,
      status_direction: DIRECTION.STOP,
      status_duration: 0,
      status_speed: new Speed(SPEED_TYPE.FACTOR, 0),
      // user input
      input_speed: new Speed(SPEED_TYPE.FACTOR, 0),
      input_direction: DIRECTION.STOP,
      // config info
      config_bssid: "",
      config_pwd: "",
      config_ratio: 0,
    };
  }

  render() {
    return (
      <div className="APP">
        <div className="btn-bar">
          <Button bsSize="large">设置</Button>
          <Button bsSize="large">{VISUAL_MODE.str(VISUAL_MODE.not(this.state.visual_mode))}</Button>
        </div>
        <div className="card-div">
          <Card>
            <Card.Body>
              <Card.Title>状态</Card.Title>
              <hr/>
              <div>
                <div className="in-card-part">
                  <div>
                    <h4>方向</h4>
                    <div>
                      <p>{DIRECTION.str_clock(this.state.status_direction)}</p>
                      <p>{DIRECTION.str(this.state.status_direction)}</p>
                    </div>
                  </div>
                  <div>
                    <h4>运行时长</h4>
                    <div>
                      <p>{this.state.status_duration}s</p>
                    </div>
                  </div>
                </div>
                <div className="in-card-part">
                  <div>
                    <h4>速度</h4>
                    <div>
                      <p>{this.state.status_speed.str_num()}</p>
                      <p>{this.state.status_speed.str()}</p>
                    </div>
                  </div>
                </div>
              </div>
            </Card.Body>
          </Card>
        </div>
        <div className="card-div">
          <Card>
            <Card.Body>
              <Card.Title>控制</Card.Title>
              <hr/>
              <div className="card-content">
                <div className="in-card-part">
                  <div>
                    <h4>速度</h4>
                    <div key={`speed_input`} className="speed_input">
                      {App.default_speed_list.map((s, i) => (
                        <Form.Check type={`radio`} name="speed_group" id={`speed-${i}`}>
                          <Form.Check.Input type={`radio`} name="speed_group" defaultChecked={i === 0}/>
                          <Form.Check.Label>{s.str_num() + '~' + s.str()}</Form.Check.Label>
                        </Form.Check>
                      ))}
                      <Form.Check type={`radio`} name="speed_group" id={`speed-custom`}>
                        <Form.Check.Input type={`radio`} name="speed_group"/>
                        <Form.Check.Label>
                          <Form.Control className={"in-check-control"} size="sm" type="number"/> <span>°/min</span>
                        </Form.Check.Label>
                      </Form.Check>
                    </div>
                  </div>
                </div>
                <div className="in-card-part">
                  <div>
                    <h4>方向</h4>
                    <div key={`direction_input`} className="direction_input">
                      <Form.Check type={`radio`} name="direction_group" id={`direction-pos`}>
                        <Form.Check.Input type={`radio`} name="direction_group" defaultChecked={true}/>
                        <Form.Check.Label>{DIRECTION.str_clock(DIRECTION.POS) + ":" + DIRECTION.str(DIRECTION.POS)}</Form.Check.Label>
                      </Form.Check>
                      <Form.Check type={`radio`} name="direction_group" id={`direction-neg`}>
                        <Form.Check.Input type={`radio`} name="direction_group"/>
                        <Form.Check.Label>{DIRECTION.str_clock(DIRECTION.NEG) + ":" + DIRECTION.str(DIRECTION.NEG)}</Form.Check.Label>
                      </Form.Check>
                    </div>
                  </div>
                </div>
              </div>
              <div className="in-card-btn-group">
                <Button>开始</Button>
                <Button>点动</Button>
              </div>
            </Card.Body>
          </Card>
        </div>
      </div>
    );
  }
}

export default App;
