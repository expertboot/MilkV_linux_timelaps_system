TARGET=rood_oled


PROJECT_NAME=$(TARGET)
# Настройки удаленного доступа к milkv
REMOTE_HOST = root@10.0.0.70
LOCAL_BINARY = $(PROJECT_NAME)
# Укажите путь к удаленному каталогу
REMOTE_PATH = /root/programs/
# Путь к файлу лога на удаленной машине
LOGFILE = $(REMOTE_PATH)logfile.log
# Команда для копирования файла через SCP
SCP_CMD=scp $(LOCAL_BINARY) $(REMOTE_HOST):$(REMOTE_PATH)/
# Команда для выполнения SSH на удаленном хосте
SSH_CMD=ssh $(REMOTE_HOST) 


ifeq (,$(TOOLCHAIN_PREFIX))
$(error TOOLCHAIN_PREFIX is not set)
endif

ifeq (,$(CFLAGS))
$(error CFLAGS is not set)
endif

ifeq (,$(LDFLAGS))
$(error LDFLAGS is not set)
endif

CC = $(TOOLCHAIN_PREFIX)gcc

CFLAGS += -I$(SYSROOT)/usr/include

LDFLAGS += -L$(SYSROOT)/lib
LDFLAGS += -L$(SYSROOT)/usr/lib
LDFLAGS += -lwiringx

SOURCE = $(shell find . -name "*.c")
#SOURCE = $(wildcard *.c)

OBJS = $(patsubst %.c,%.o,$(SOURCE))

start:
	$(MAKE) clean && \
	$(MAKE) all && \
	$(MAKE) prog-start


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	@rm *.o -rf
	@rm $(OBJS) -rf
	@rm $(TARGET)

# Команда для перезапуска программы
prog-start:
	# Проверяем, запущен ли процесс
	@if $(SSH_CMD) "ps | grep $(PROJECT_NAME) | grep -v grep > /dev/null"; then \
		echo "Завершаю процесс $(PROJECT_NAME)..."; \
		$(SSH_CMD) "ps | grep $(PROJECT_NAME) | grep -v grep | awk '{print \$$1}' | xargs kill"; \
    fi
	# Проверяем, существует ли локальный бинарный файл
	@if [ -f $(LOCAL_BINARY) ]; then \
        echo "Копирую $(LOCAL_BINARY) на удалённый хост..."; \
        $(SCP_CMD); \
        echo "Запускаю программу на $(REMOTE_HOST)..."; \
        $(SSH_CMD) "nohup stdbuf -oL -eL $(REMOTE_PATH)/$(PROJECT_NAME) > $(REMOTE_PATH)/logfile.log 2>&1 &"; \
    else \
        echo "Бинарный файл $(LOCAL_BINARY) не найден!"; \
    fi

# Функция для завершения программы на удалённом хосте
prog-stop:
	@if $(SSH_CMD) "ps | grep $(PROJECT_NAME) | grep -v grep > /dev/null"; then \
        echo "Завершаю процесс $(PROJECT_NAME)..."; \
        $(SSH_CMD) "ps | grep $(PROJECT_NAME) | grep -v grep | awk '{print \$$1}' | xargs kill"; \
    else \
        echo "Процесс $(PROJECT_NAME) не найден на $(REMOTE_HOST)"; \
    fi


# Функция для считывания логов с удалённого хоста
prog-logs:
	@echo "Чтение логов с $(REMOTE_HOST)..."
	$(SSH_CMD) "tail -f $(REMOTE_LOG_FILE)"

# Функция для запуска программы и открытия отдельного терминала для логов
prog-start-log:
	@echo "Запуск программы $(PROJECT_NAME) и открытие логов..."
	$(MAKE) prog-start
	@ssh $(REMOTE_HOST) 'tail -f $(LOGFILE)'
