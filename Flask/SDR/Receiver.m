stopFilePath = 'stop_receiver.txt';
if exist(stopFilePath, 'file')
    delete(stopFilePath);
end

rc = "DL-FRC-FR1-64QAM";
bw = "10MHz";
scs = "30kHz";
dm = "FDD";
rnti = 0;

rx = hSDRReceiver("Pluto");
rx.CenterFrequency = 3.4e9;
rx.SampleRate = 30.72e6;
rx.Gain = 60;

spectrumPlotRx = spectrumAnalyzer;
spectrumPlotRx.SampleRate = rx.SampleRate;
spectrumPlotRx.SpectrumType = "Power density";
spectrumPlotRx.YLabel = "PSD";
spectrumPlotRx.Title = "Received Signal Spectrum";

captureDuration = 0.01;

try
    while true
        if exist(stopFilePath, 'file')
            disp('Stop signal detected. Stopping receiver.');
            break;
        end

        rxWaveform = capture(rx, seconds(captureDuration));
        spectrumPlotRx(rxWaveform);

        pause(0.01);
    end
catch ME
    disp('오류 발생:');
    disp(ME.message);
end

try
    release(rx);
    release(spectrumPlotRx);
catch
    disp('리소스 해제 중 오류 발생');
end

disp('Receiver 작업 중지됨.');