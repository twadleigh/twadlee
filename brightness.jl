# brightness table

const max_pwm = 256
const num_intervals = 30

# brightness to luminance
function finv(t)
    δ = 6/29
    ϵ = 4/29
    t > δ ? t^3 : 3*δ^2*(t-ϵ)
end
luminance(brightness) = max_pwm*finv((brightness+16)/116)
brightnesses = 100/(num_intervals)*(0:num_intervals)

# table of luminance in steps of equal brightness
luminances = [round(Int,luminance(b)) for b in brightnesses]

ls = join([repr(l) for l in luminances], ", ")

open("LuminanceTable.h", "w") do io
    print(io,
        """
        const uint8_t kMaxBrightness = $num_intervals;
        const uint16_t kLuminances[$(num_intervals+1)] = {$ls};
        """)
end
