import React from 'react';
import { Card, Button, Form, Input, Row, Col } from 'antd';

const Connection = ({ connect, disconnect, connectBtn }) => {
  const [form] = Form.useForm();
  const record = {
    host: '10.0.0.101',
    clientId: `mqttjs_${Math.random().toString(16).substr(2, 8)}`,
    port: 9001,
    username: "aluno",
    password: "aluno*123"
  };
  const onFinish = (values) => {
    const { host, clientId, port, username, password } = values;
    const url = `ws://${host}`;
    console.log(url);
    const options = {
      protocol: 'ws',
      port: port,
      clientId: clientId,
      username: username,
      password: password,
      rejectUnauthorized: false,
    };
    connect(url, options);
  };

  const handleConnect = () => {
    form.submit();
  };

  const handleDisconnect = () => {
    disconnect();
  };

  const ConnectionForm = (
    <Form
      layout="vertical"
      name="basic"
      form={form}
      initialValues={record}
      onFinish={onFinish}
    >
      <Row gutter={20}>
        <Col span={8}>
          <Form.Item
            label="Host"
            name="host"
          >
            <Input />
          </Form.Item>
        </Col>
        <Col span={8}>
          <Form.Item
            label="Port"
            name="port"
          >
            <Input />
          </Form.Item>
        </Col>
        <Col span={8}>
          <Form.Item
            label="Client ID"
            name="clientId"
          >
            <Input />
          </Form.Item>
        </Col>
        <Col span={8}>
          <Form.Item
            label="Username"
            name="username"
          >
            <Input />
          </Form.Item>
        </Col>
        <Col span={8}>
          <Form.Item
            label="Password"
            name="password"
          >
            <Input />
          </Form.Item>
        </Col>
      </Row>
    </Form>
  )

  return (
    <Card
      title="Conexão"
      actions={[
        <Button type="primary" onClick={handleConnect}>{connectBtn}</Button>,
        <Button danger onClick={handleDisconnect}>Disconnect</Button>
      ]}
    >
      {ConnectionForm}
    </Card>
  );
}

export default Connection;
