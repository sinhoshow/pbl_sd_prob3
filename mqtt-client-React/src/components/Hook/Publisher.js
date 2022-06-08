import React, { useContext } from 'react';
import { Card, Form, Input, Row, Col, Button, Select } from 'antd';
import { QosOption } from './index'

const Publisher = ({ publish }) => {
  const [form] = Form.useForm();
  const qosOptions = useContext(QosOption);

  const record = {
    topic: 'tp_03_g04/intervalo',
    qos: 0,
  };

  const onFinish = (values) => {
    publish({ ...record, ...values })
  };

  const PublishForm = (
    <Form
      layout="vertical"
      name="basic"
      form={form}
      initialValues={record}
      onFinish={onFinish}
    >
      <Row gutter={20}>
        <Col span={24}>
          <Form.Item
            label="Selecione um intervalo de tempo (em s):"
            name="payload"
          >
            <Input />
          </Form.Item>
        </Col>
        <Col span={8} offset={16} style={{ textAlign: 'right' }}>
          <Form.Item>
            <Button type="primary" htmlType="submit">
              Selecionar
            </Button>
          </Form.Item>
        </Col>
      </Row>
    </Form>
  )

  return (
    <Card
      title="Intervalo de tempo"
    >
      {PublishForm}
    </Card>
  );
}

export default Publisher;
