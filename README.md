# STM32 Smart Watch

STM32F411RE(NUCLEO-F411RE) 기반 멀티 기능 스마트 워치 펌웨어입니다.
버튼 하나로 **시계 → 온습도 → 초음파 거리 → 스톱워치** 4가지 모드를 순환하며, 결과는 I2C 16x2 LCD에 표시하고 버튼 입력과 초 단위 동작은 부저 소리로 알려 줍니다.

## 주요 기능

| 모드 | 동작 | 버튼 |
|---|---|---|
| 시계 (TIME_CLOCK) | DS3231 RTC에서 날짜·요일·시간을 읽어 LCD 표시, 1초마다 tick 음 | BTN1: 다음 모드 |
| 온습도 (TEMP_HUMID) | DHT11 측정값 표시 (`T:xx.xC H:xx%`), 체크섬 오류 표시 | BTN2: 측정 |
| 거리 (DISTANCE) | HC-SR04 초음파 거리(cm) 측정, 측정중/OK/ERROR 상태 표시 | BTN2: 측정 |
| 스톱워치 (STOP_WATCH) | 10ms 해상도 스톱워치, 동작 중 1초마다 tick 음 | BTN2: Run/Stop, BTN3: Clear |

## 소프트웨어 구조

애플리케이션 → 서비스 → 드라이버의 3계층으로 나눠 하드웨어 의존 코드를 분리했습니다.
각 모드(state) 함수는 **① 버튼 이벤트 처리 → ② 서비스 처리 → ③ LCD 출력** 순서로 동작합니다.

```
Core/Src
├── ap/                      # Application
│   ├── ap_main.c            # 모드 FSM, 타이머/EXTI 콜백
│   ├── service/             # 기능 단위 로직
│   │   ├── TimeClock/       # RTC 시간/날짜
│   │   ├── StopWatch/       # 스톱워치 (10ms tick)
│   │   ├── temp_humid/      # 온습도 측정
│   │   ├── distance/        # 초음파 거리 측정
│   │   ├── display/         # LCD 화면 구성 (변경된 줄만 갱신)
│   │   ├── sound/           # 부저 효과음
│   │   └── btn_event/       # 버튼 이벤트 → 큐
│   └── common/              # delay(us), 원형 큐 버퍼
└── driver/                  # 하드웨어 드라이버
    ├── lcd/                 # I2C LCD (PCF8574, 0x27)
    ├── ds3231/              # I2C RTC
    ├── dht11/               # 온습도 센서 (1-wire 타이밍)
    ├── hcsr04/              # 초음파 센서
    ├── buzzer/              # PWM 부저
    ├── button/  led/  fnd/
```

설계 포인트
- **이벤트 큐 기반 버튼 처리**: 버튼 입력을 큐에 쌓고 메인 루프에서 하나씩 꺼내 처리해서, 입력이 누락되거나 모드 전환과 꼬이지 않게 했습니다.
- **인터럽트 분리**: TIM4(10ms) 인터럽트에서는 시계·스톱워치 카운트만 하고, 무거운 처리(LCD, 센서)는 메인 루프에서 수행합니다.
- **HC-SR04 Echo 측정**: Echo 핀(PC6)의 상승/하강 엣지를 EXTI로 받고 TIM10으로 펄스 폭을 재서 블로킹 없이 거리를 계산합니다.
- **LCD 갱신 최소화**: 이전에 출력한 내용과 비교해 바뀐 줄만 다시 씁니다.

## 하드웨어 구성

| 부품 | 인터페이스 | 핀 |
|---|---|---|
| 16x2 LCD (I2C 백팩) | I2C1 | PB8(SCL), PB9(SDA) |
| DS3231 RTC | I2C1 | PB8(SCL), PB9(SDA) |
| DHT11 | GPIO | PC8 |
| HC-SR04 | GPIO + EXTI + TIM10 | PC5(Trig), PC6(Echo) |
| 부저 | TIM1 CH4 PWM | PA11 |
| 버튼 1/2/3 | GPIO 입력 | PC0, PC1, PC3 |

- MCU: STM32F411RET6, SYSCLK 100 MHz
- 타이머: TIM1(부저 PWM), TIM4(10ms 시스템 tick), TIM10(초음파 펄스 측정)

## 빌드 및 실행

1. STM32CubeIDE에서 `File → Import → Existing Projects into Workspace`로 이 폴더를 엽니다.
2. `Weekend_HW.ioc`로 핀 설정을 확인할 수 있습니다. 코드를 재생성하는 경우 `USER CODE` 영역 밖의 수정 사항은 덮어써질 수 있습니다.
3. Build 후 NUCLEO-F411RE에 Debug/Run 합니다.

## 개발 환경

- STM32CubeIDE, STM32 HAL (STM32F4xx)
- C
