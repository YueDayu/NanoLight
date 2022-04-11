import React from 'react';

import {Button, Card, Form, Modal} from 'react-bootstrap';

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

  static classname(mode) {
    if (mode === this.DARK) {
      return "red_mode";
    }
    return "";
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
        return "STOP";
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
      return (Math.round(output_speed / 60 * 100) / 100) + "°/min"
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
  constructor(props) {
    super(props);
    this.default_speed_list = [
      new Speed(SPEED_TYPE.FACTOR, 1),
      new Speed(SPEED_TYPE.FACTOR, 5),
      new Speed(SPEED_TYPE.FACTOR, 10),
      new Speed(SPEED_TYPE.FACTOR, 100)];

    this.default_direction_list = [
      DIRECTION.POS,
      DIRECTION.NEG];

    this.state = {
      // visual config
      visual_mode: VISUAL_MODE.NORMAL,
      // display info, fetch from server
      // status_fetched: false,
      status_direction: DIRECTION.STOP,
      status_duration: 0,
      status_speed: new Speed(SPEED_TYPE.FACTOR, 0),
      // user input
      input_speed: this.default_speed_list[0],
      input_direction: this.default_direction_list[0],
      // config info
      config_fetched: true,
      config_ssid: "",
      config_pwd: "",
      config_ratio: 100,
      config_ssid_user: "",
      config_pwd_user: "",
      config_ratio_user: 0,
      config_display: false,
      // connection
      connection: true,
    };
  }

  ///////> network handler

  ///////> element callback
  trigger_visual_mode() {
    this.setState({visual_mode: VISUAL_MODE.not(this.state.visual_mode)});
  }

  trigger_config() {
    this.setState({config_display: !this.state.config_display});
  }

  close_config() {
    this.setState({
      config_display: false,
      config_ssid_user: "",
      config_pwd_user: "",
      config_ratio_user: 0,
    })
    if (this.state.config_fetched) {
      this.config_input_form_ref.reset();
    }
  }

  handle_config_ssid_change(event) {
    this.setState({config_ssid_user: event.target.value});
  }

  handle_config_pwd_change(event) {
    this.setState({config_pwd_user: event.target.value});
  }

  handle_config_ratio_change(event) {
    let val = parseFloat(event.target.value);
    val = Math.max(Math.min(9999, val), -9999);
    this.setState({config_ratio_user: val});
  }

  handle_direction_check(dir, event) {
    this.setState({input_direction: dir});
  }

  handle_speed_check(speed, event) {
    this.setState({input_speed: speed});
  }

  handle_speed_input(event) {
    this.speed_check_radio.checked = true;
    if (event.target.value === "") {
      this.setState({input_speed: new Speed(SPEED_TYPE.DEGREE, 0)});
    } else {
      this.setState({input_speed: new Speed(SPEED_TYPE.DEGREE, parseFloat(event.target.value) * 60.)});
    }
  }

  handle_speed_radio(event) {
    let v = this.speed_check_control.value;
    if (v === "") {
      this.setState({input_speed: new Speed(SPEED_TYPE.DEGREE, 0)});
    } else {
      this.setState({input_speed: new Speed(SPEED_TYPE.DEGREE, parseFloat(v) * 60.)});
    }
  }

  handle_start_btn(event) {
    console.log("start");
  }

  handle_btn_push(event) {
    console.log("btn down");
  }

  handle_btn_release(event) {
    console.log("btn up");
  }

  handle_config_save(event) {
    console.log("save config");
  }

  render() {
    return (
      <div className={"APP " + VISUAL_MODE.classname(this.state.visual_mode)}>
        <div className="btn-bar">
          <Button bsSize="large" disabled={this.state.status_direction !== DIRECTION.STOP}
                  onClick={this.trigger_config.bind(this)}>设置</Button>
          <Button bsSize="large"
                  onClick={this.trigger_visual_mode.bind(this)}>{VISUAL_MODE.str(VISUAL_MODE.not(this.state.visual_mode))}</Button>
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
                    <div className="info-pad">
                      <p>{DIRECTION.str_clock(this.state.status_direction)}</p>
                      <p>{DIRECTION.str(this.state.status_direction)}</p>
                    </div>
                  </div>
                  <div>
                    <h4>运行时长</h4>
                    <div className="info-pad">
                      <p>{(this.state.status_duration >= 60 ?
                          (Math.floor(this.state.status_duration / 60) + '分钟') : "")
                        + this.state.status_duration % 60 + '秒'}</p>
                    </div>
                  </div>
                </div>
                <div className="in-card-part">
                  <div>
                    <h4>速度</h4>
                    <div className="info-pad">
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
                      {this.default_speed_list.map((s, i) => (
                        <Form.Check type={`radio`} name="speed_group" id={`speed-${i}`}>
                          <Form.Check.Input type={`radio`} name="speed_group"
                                            onChange={this.handle_speed_check.bind(this, s)} defaultChecked={i === 0}/>
                          <Form.Check.Label>{s.str_num() + ' ' + s.str()}</Form.Check.Label>
                        </Form.Check>
                      ))}
                      <Form.Check type={`radio`} name="speed_group" id={`speed-custom`}>
                        <Form.Check.Input ref={(el) => {
                          this.speed_check_radio = el;
                        }} onChange={this.handle_speed_radio.bind(this)} type={`radio`} name="speed_group"/>
                        <Form.Check.Label>
                          <Form.Control ref={(el) => {
                            this.speed_check_control = el;
                          }} onChange={this.handle_speed_input.bind(this)} className={"in-check-control"} size="sm"
                                        type="number"/> <span>°/min</span>
                        </Form.Check.Label>
                      </Form.Check>
                    </div>
                  </div>
                </div>
                <div className="in-card-part">
                  <div>
                    <h4>方向</h4>
                    <div key={`direction_input`} className="direction_input">
                      {this.default_direction_list.map((d, i) => (
                        <Form.Check type={`radio`} name="direction_group" id={"direction-" + d}>
                          <Form.Check.Input type={`radio`} name="direction_group" defaultChecked={i === 0}
                                            onChange={this.handle_direction_check.bind(this, d)}/>
                          <Form.Check.Label>{DIRECTION.str_clock(d) + ":" + DIRECTION.str(d)}</Form.Check.Label>
                        </Form.Check>
                      ))}
                    </div>
                  </div>
                </div>
              </div>
              <div className="in-card-btn-group">
                <Button
                  onClick={this.handle_start_btn.bind(this)}>{this.state.status_direction === DIRECTION.STOP ? "开始" : "停止"}</Button>
                <Button disabled={this.state.status_direction !== DIRECTION.STOP}
                        onTouchStart={this.handle_btn_push.bind(this)}
                        onMouseDown={this.handle_btn_push.bind(this)}
                        onTouchEnd={this.handle_btn_release.bind(this)}
                        onMouseUp={this.handle_btn_release.bind(this)}>点动</Button>
              </div>
            </Card.Body>
          </Card>
        </div>
        <Modal className={VISUAL_MODE.classname(this.state.visual_mode)}
               show={this.state.connection && this.state.config_display} keyboard={false}
               onHide={this.close_config.bind(this)}>
          <Modal.Header closeButton>
            <Modal.Title>设置</Modal.Title>
          </Modal.Header>
          <Modal.Body>
            <Form ref={(el) => {
              this.config_input_form_ref = el;
            }}>
              <Form.Group className="mb-3" controlId="formBasicEmail">
                <Form.Label>SSID</Form.Label>
                <Form.Control type="text" disabled={!this.state.config_fetched} maxLength={10}
                              placeholder={this.state.config_fetched ? this.state.config_ssid : "获取中……"}
                              onChange={this.handle_config_ssid_change.bind(this)}/>
              </Form.Group>
              <Form.Group className="mb-3" controlId="formBasicPassword">
                <Form.Label>密码</Form.Label>
                <Form.Control type="text" disabled={!this.state.config_fetched} maxLength={10}
                              placeholder={this.state.config_fetched ? this.state.config_pwd : "获取中……"}
                              onChange={this.handle_config_pwd_change.bind(this)}/>
                <small className="form-text text-muted">WiFi设置将在重启后生效，SSID与密码最长十位</small>
              </Form.Group>
              <Form.Group className="mb-3" controlId="formBasicPassword">
                <Form.Label>减速比</Form.Label>
                <Form.Control type="number" disabled={!this.state.config_fetched}
                              placeholder={this.state.config_fetched ? this.state.config_ratio : "获取中……"}
                              onChange={this.handle_config_ratio_change.bind(this)}/>
                <small className="form-text text-muted">默认减速比为2916，反转减速箱加负号即可；最长十位</small>
              </Form.Group>
            </Form>
          </Modal.Body>
          <Modal.Footer>
            <Button onClick={this.close_config.bind(this)}>关闭</Button>
            <Button onClick={this.handle_config_save.bind(this)}>保存</Button>
          </Modal.Footer>
        </Modal>
        <Modal className={VISUAL_MODE.classname(this.state.visual_mode)} show={!this.state.connection} backdrop="static"
               keyboard={false}>
          <Modal.Header>
            <Modal.Title>未连接</Modal.Title>
          </Modal.Header>
          <Modal.Body>
            等待重连……
          </Modal.Body>
        </Modal>
      </div>
    );
  }
}

export default App;
